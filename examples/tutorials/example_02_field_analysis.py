"""
First field analysis — iterate over time steps
==============================================

The previous tutorial read a single field at one instant. Most
post-processing tasks want to loop over a sequence of steps and build
a statistic per step — track a residual, a mean, a maximum.

In this tutorial we drive the OpenFOAM time loop ourselves
(``while time.loop(): …``), update ``U`` in place through a zero-copy
NumPy view, and collect a summary table row-by-row.

Prerequisite: finish :doc:`example_01_getting_started` so the
``pybFoam.Time`` / ``fvMesh`` pattern is familiar.
"""

# %%
# Prepare the case
# ----------------
# We use ``examples/cavity/`` — a minimal icoFoam setup with just ``U``
# and ``p``. We clone to a tmp copy and shorten ``endTime`` so the loop
# runs in a fraction of a second.

import shutil
import tempfile
from pathlib import Path

import pybFoam.pybFoam_core as core
from pybFoam import meshing


def _examples_root() -> Path:
    for p in [Path.cwd(), *Path.cwd().parents]:
        if p.name == "examples":
            return p
    raise RuntimeError("Could not locate examples/ root")


BASELINE = (_examples_root() / "cavity").resolve()
CASE = Path(tempfile.mkdtemp(prefix="pybfoam_field_analysis_")) / "cavity"
shutil.copytree(BASELINE, CASE)

# Shorten the run to five time steps (deltaT=0.001, endTime=0.005).
cd_path = CASE / "system" / "controlDict"
cd_text = cd_path.read_text()
cd_text = cd_text.replace("endTime         0.5;", "endTime         0.005;")
cd_text = cd_text.replace("deltaT          0.0005;", "deltaT          0.001;")
cd_path.write_text(cd_text)

zero, zero_orig = CASE / "0", CASE / "0.orig"
if zero.exists():
    shutil.rmtree(zero)
shutil.copytree(zero_orig, zero)

args = core.argList([str(CASE), "-case", str(CASE)])
time = core.Time(args)
mesh = meshing.generate_blockmesh(time, core.dictionary.read(f"{CASE}/system/blockMeshDict"))

# %%
# Read the field once
# -------------------
# ``read_field`` returns a ``volVectorField`` that lives for the rest
# of the script. ``np.asarray(U["internalField"])`` is a zero-copy view
# — modifications through that view are visible to every OpenFOAM
# routine holding the same field.

import numpy as np

from pybFoam import volVectorField

U = volVectorField.read_field(mesh, "U")
U_view = np.asarray(U["internalField"])

print(f"nCells = {mesh.nCells()}   U shape = {U_view.shape}")

# %%
# Drive the time loop
# -------------------
# ``time.loop()`` advances the clock by ``deltaT`` and returns ``False``
# when ``endTime`` is reached. At each step we nudge ``U`` (fake a
# solver step), compute ``|U|`` through NumPy, and record a row.

rows = []
while time.loop():
    t = time.value()
    # Fake a per-step update: scale the x-component with time.
    U_view[:, 0] = 0.5 * t
    u_mag = np.linalg.norm(U_view, axis=1)
    rows.append((t, float(u_mag.mean()), float(u_mag.max())))

# %%
# Summary per step
# ----------------

print(f"{'t':>8s}  {'mean|U|':>10s}  {'max|U|':>10s}")
for t, umean, umax in rows:
    print(f"{t:8.4f}  {umean:10.4f}  {umax:10.4f}")

# %%
# What's next
# -----------
#
# * :doc:`example_03_sampling_workflow` — restrict the analysis to a
#   plane or line instead of the whole volume.
# * :doc:`/auto_how_to/fvc_fvm/example_fvc_fvm_operators` — derive new
#   fields (``grad``, ``div``, ``laplacian``) from the ones you read.
