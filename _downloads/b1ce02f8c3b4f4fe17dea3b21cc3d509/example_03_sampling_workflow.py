"""
First sampling workflow — plane to CSV
======================================

The previous tutorials read fields across the whole volume. Often you
only care about a slice — a plane through the domain, a line along an
axis, values on a boundary patch.

This tutorial defines a sampling plane with a Pydantic config,
interpolates a field onto it, and exports the result as CSV. The same
pattern works for isosurfaces, patches, and uniform lines.

Prerequisite: :doc:`example_01_getting_started` sets up the case +
mesh + field pattern you will see again here.
"""

# %%
# Prepare the case
# ----------------

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


BASELINE = (_examples_root() / "case").resolve()
CASE = Path(tempfile.mkdtemp(prefix="pybfoam_sampling_tutorial_")) / "case"
shutil.copytree(BASELINE, CASE)

zero, zero_orig = CASE / "0", CASE / "0.orig"
if zero.exists():
    shutil.rmtree(zero)
shutil.copytree(zero_orig, zero)

args = core.argList([str(CASE), "-case", str(CASE)])
time = core.Time(args)
mesh = meshing.generate_blockmesh(time, core.dictionary.read(f"{CASE}/system/blockMeshDict"))

# %%
# Describe the sampling plane
# ---------------------------
# Sampling geometries in pybFoam are typed Pydantic configs — you get
# IDE autocompletion and field validation instead of hand-writing an
# OpenFOAM dictionary string.

from pybFoam.sampling import SampledPlaneConfig

config = SampledPlaneConfig(
    point=[0.5, 0.5, 0.0],
    normal=[1.0, 0.0, 0.0],
)

# %%
# Create the surface
# ------------------
# The config converts to a native OpenFOAM ``dictionary`` with
# ``to_foam_dict()``; :meth:`sampledSurface.New` is the factory.

from pybFoam import Word
from pybFoam.sampling import sampledSurface

plane = sampledSurface.New(Word("midPlane"), mesh, config.to_foam_dict())
plane.update()  # must be called before reading geometry

print(f"faces : {len(plane.magSf())}")
print(f"area  : {plane.area():.4f}")

# %%
# Interpolate a scalar field
# --------------------------
# Three interpolation schemes are available — ``"cell"`` (fastest,
# piecewise-constant), ``"cellPoint"`` (continuous linear, good
# default), and ``"cellPointFace"`` (most accurate).

import numpy as np

from pybFoam import volScalarField
from pybFoam.sampling import interpolationScalar, sampleOnFacesScalar

p_rgh = volScalarField.read_field(mesh, "p_rgh")

interp = interpolationScalar.New(Word("cellPoint"), p_rgh)
sampled = sampleOnFacesScalar(plane, interp)

centers = np.asarray(plane.Cf())
values = np.asarray(sampled)
print(f"values : {values.shape}")
print(f"centers: {centers.shape}")

# %%
# Export to CSV
# -------------
# ``csv`` from the standard library is enough for most cases; pandas
# is a one-line substitute. We write to a tmp path so the tutorial
# leaves nothing behind.

import csv

out = Path(tempfile.gettempdir()) / "midPlane.csv"
with out.open("w", newline="") as f:
    w = csv.writer(f)
    w.writerow(["x", "y", "z", "p_rgh"])
    for (x, y, z), v in zip(centers, values):
        w.writerow([x, y, z, v])

print(f"wrote {out}")
print(out.read_text().splitlines()[0])
for line in out.read_text().splitlines()[1:4]:
    print(line)

# %%
# Swap the geometry
# -----------------
# Once the plane pattern clicks, swap the config to sample entirely
# different geometries without touching any downstream code.

from pybFoam.sampling import SampledPatchConfig

patch_cfg = SampledPatchConfig(patches=["atmosphere"])
patch_surf = sampledSurface.New(Word("atmPatch"), mesh, patch_cfg.to_foam_dict())
patch_surf.update()
print(f"atmosphere patch: {len(patch_surf.magSf())} faces")

# %%
# What's next
# -----------
#
# * :doc:`/auto_how_to/sampling/example_sample_line` — values along a
#   uniform line instead of a surface.
# * :doc:`/auto_how_to/fvc_fvm/example_fvc_fvm_operators` — derive a
#   new field (gradient / divergence / Laplacian) first, then sample
#   *that*.
