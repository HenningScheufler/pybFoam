"""
Sample a field on a plane
=========================

Define a sampling plane with :class:`pybFoam.sampling.SampledPlaneConfig`,
create the surface through the :func:`sampledSurface.New` factory, and
interpolate a scalar field onto it.
"""

# %%
# Prepare a working case
# ----------------------
# The ``examples/`` tree is the documentation baseline — clone the case
# into a tmp directory, restore ``0/``, then build the mesh there.

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
CASE = Path(tempfile.mkdtemp(prefix="pybfoam_sample_plane_")) / "case"
shutil.copytree(BASELINE, CASE)

zero = CASE / "0"
if zero.exists():
    shutil.rmtree(zero)
shutil.copytree(CASE / "0.orig", zero)

args = core.argList([str(CASE), "-case", str(CASE)])
time = core.Time(args)
mesh = meshing.generate_blockmesh(time, core.dictionary.read(f"{CASE}/system/blockMeshDict"))

# %%
# Define the plane
# ----------------
# The Pydantic config validates the fields at construction and converts
# to an OpenFOAM dictionary on demand.

from pybFoam import Word
from pybFoam.sampling import SampledPlaneConfig, sampledSurface

cfg = SampledPlaneConfig(point=[0.5, 0.5, 0.0], normal=[1.0, 0.0, 0.0])
plane = sampledSurface.New(Word("midPlane"), mesh, cfg.to_foam_dict())
plane.update()

print(f"faces: {len(plane.magSf())}   area: {plane.area():.4f}")

# %%
# Interpolate a scalar field
# --------------------------
# Choose the scheme: ``"cell"`` (piecewise-constant, fastest),
# ``"cellPoint"`` (continuous linear, good default), or
# ``"cellPointFace"`` (most accurate, higher cost).

import numpy as np

from pybFoam import volScalarField
from pybFoam.sampling import interpolationScalar, sampleOnFacesScalar

p_rgh = volScalarField.read_field(mesh, "p_rgh")

interp = interpolationScalar.New(Word("cellPoint"), p_rgh)
sampled = sampleOnFacesScalar(plane, interp)

centers = np.asarray(plane.Cf())
values = np.asarray(sampled)
print(f"values.shape  = {values.shape}")
print(f"centers.shape = {centers.shape}")

# %%
# Sample a vector field on the same surface
# -----------------------------------------

from pybFoam import volVectorField
from pybFoam.sampling import interpolationVector, sampleOnFacesVector

U = volVectorField.read_field(mesh, "U")

interp_U = interpolationVector.New(Word("cellPoint"), U)
sampled_U = sampleOnFacesVector(plane, interp_U)

print(f"sampled_U.shape = {np.asarray(sampled_U).shape}")

# %%
# Swap the geometry
# -----------------
# The same pattern works for any of the surface kinds — patches,
# isosurfaces, cutting planes, distance surfaces — by swapping the
# config:

from pybFoam.sampling import SampledPatchConfig

patch_cfg = SampledPatchConfig(patches=["atmosphere"])
patch_surf = sampledSurface.New(Word("atmPatch"), mesh, patch_cfg.to_foam_dict())
patch_surf.update()
print(f"atmosphere patch: {len(patch_surf.magSf())} faces")
