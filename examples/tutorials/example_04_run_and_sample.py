"""
Run the cavity case and sample a line over time
===============================================

By the end of this tutorial you will have:

- truncated the cavity's runtime by editing ``system/controlDict``
  (the modify-and-write pattern from :doc:`example_01_dictionaries`),
- driven OpenFOAM's icoFoam PISO loop one step at a time from Python,
- sampled velocity on a horizontal mid-line of the cavity at every
  step, and
- plotted a ``|U|(x, t)`` heatmap together with a final pyvista
  snapshot of the cavity flow.

The skill on display is **interleaving** Python work with the time
loop: between two PISO steps, you can read state, sample, write a
file, redraw a UI, or hand control to a coupling library.

Prerequisites
-------------

- Finished :doc:`example_03_modify_initial_conditions`.
- The icoFoam solver class lives in
  ``examples/cavity/icoFoam.py``; we import it directly.
"""

# %%
# Clone the case
# --------------

from pybFoam import clone_case

case = clone_case("cavity")
print(f"case = {case}")

# %%
# Truncate the runtime
# --------------------
# Default cavity ``endTime`` is 0.5 s — too long for a doc build.
# Use the dictionary-modify pattern from T1 to dial it down. Five
# steps of size ``deltaT = 0.001`` is enough to see the lid-driven
# circulation forming.

from pybFoam import dictionary

control_dict_path = case / "system" / "controlDict"
cd = dictionary.read(str(control_dict_path))
cd.set("endTime", 0.005)
cd.set("deltaT", 0.001)
cd.set("writeInterval", 0.005)
cd.write(str(control_dict_path))

cd_check = dictionary.read(str(control_dict_path))
print("endTime       =", cd_check.get[float]("endTime"))
print("deltaT        =", cd_check.get[float]("deltaT"))

# %%
# Import the solver
# -----------------
# ``IcoFoam`` is shipped under ``examples/cavity/icoFoam.py``.
# Tutorials are not on Python's import path by default, so we add the
# cavity directory and import from it. :func:`pybFoam.examples_root`
# resolves the in-repo ``examples/`` folder.

import sys

from pybFoam import examples_root

sys.path.insert(0, str(examples_root() / "cavity"))
from icoFoam import IcoFoam  # noqa: E402

solver = IcoFoam(case)
print(f"nCells = {solver.mesh.nCells()}   t0 = {solver.time.value()}")

# %%
# Configure a sampling line
# -------------------------
# A single straight line from the left wall to the right wall at
# mid-height, parameterised by 50 points. The ``sampledSet`` is
# decoupled from any specific field — we build it once and use the
# same line for every step.

from pybFoam import Word
from pybFoam.sampling import (
    UniformSetConfig,
    interpolationVector,
    meshSearch,
    sampledSet,
    sampleSetVector,
)

L = 0.1  # cavity edge length
N = 50

search = meshSearch(solver.mesh)
line_cfg = UniformSetConfig(
    axis="distance",
    start=[0.0, 0.5 * L, 0.5 * 0.01],
    end=[L, 0.5 * L, 0.5 * 0.01],
    nPoints=N,
)
line = sampledSet.New(Word("midLine"), solver.mesh, search, line_cfg.to_foam_dict())

import numpy as np

distance = np.asarray(line.distance())
print(f"sample points : {len(distance)}   range : {distance[0]:.3f} … {distance[-1]:.3f}")

# %%
# Run the solver and sample on every step
# ---------------------------------------
# ``solver.step()`` advances the PISO loop by one ``deltaT`` and
# returns ``False`` once ``endTime`` is reached. Between calls we
# rebuild the interpolator so it sees the current ``solver.U`` and
# sample the line.

times: list[float] = []
profiles: list[np.ndarray] = []

while solver.step():
    interp = interpolationVector.New(Word("cellPoint"), solver.U)
    u_line = np.asarray(sampleSetVector(line, interp))
    times.append(solver.time.value())
    profiles.append(np.linalg.norm(u_line, axis=1).copy())

profiles_arr = np.stack(profiles)  # shape (n_steps, n_points)
print(f"profiles.shape = {profiles_arr.shape}")
print(f"|U| max over all steps = {profiles_arr.max():.4f} m/s")

# %%
# Plot ``|U|(x, t)`` as a heatmap
# -------------------------------
# matplotlib ``pcolormesh`` with sample distance on the x-axis and
# simulation time on the y-axis. The lid is at the top of the cavity,
# so values rise as the upper region accelerates and momentum diffuses
# downward along the line at mid-height.

import matplotlib.pyplot as plt

fig, ax = plt.subplots(figsize=(7, 3.2))
mesh = ax.pcolormesh(
    distance,
    times,
    profiles_arr,
    cmap="magma",
    shading="auto",
)
ax.set_xlabel("distance along line  [m]")
ax.set_ylabel("time  [s]")
ax.set_title("|U|  along the cavity mid-line")
fig.colorbar(mesh, ax=ax, label="|U|  [m/s]")
fig.tight_layout()
plt.show()

# %%
# Snapshot the final state with pyvista
# -------------------------------------
# At ``endTime`` the solver wrote a complete time directory; we open
# the case as a pyvista reader and render a slice through the front
# face for spatial context.

import pyvista as pv

from pybFoam import open_case

reader = open_case(case, time=solver.time.value())
internal = reader.read()["internalMesh"]
internal.set_active_vectors("U")
slice_mid = internal.slice(normal="z", origin=(0.5 * L, 0.5 * L, 0.5 * 0.01))

plotter = pv.Plotter(window_size=(640, 540), off_screen=True)
plotter.add_mesh(
    slice_mid,
    scalars="U",
    cmap="viridis",
    show_edges=False,
    scalar_bar_args={"title": "|U|  [m/s]"},
)
plotter.add_mesh(
    slice_mid.glyph(orient="U", scale="U", factor=0.05, tolerance=0.04),
    color="white",
    line_width=1,
)
plotter.view_xy()
plotter.show()

# %%
# What's next
# -----------
#
# - :doc:`/auto_how_to/example_sample_plane` — sample a 2-D slice
#   instead of a 1-D line, useful for whole-domain diagnostics.
# - :doc:`/auto_how_to/example_fvc_fvm_operators` — build new derived
#   fields (gradients, divergences, Laplacians) from the ones the
#   solver computed.
# - Substitute a different solver: any class that exposes a
#   ``step()`` method and updates a public ``U`` field can drop into
#   this loop.
