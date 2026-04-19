"""
Sample a field along a line
===========================

Define a uniformly-spaced line with
:class:`pybFoam.sampling.UniformSetConfig`, create the set via
:func:`sampledSet.New`, and read values along it.
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
CASE = Path(tempfile.mkdtemp(prefix="pybfoam_sample_line_")) / "case"
shutil.copytree(BASELINE, CASE)

zero = CASE / "0"
if zero.exists():
    shutil.rmtree(zero)
shutil.copytree(CASE / "0.orig", zero)

args = core.argList([str(CASE), "-case", str(CASE)])
time = core.Time(args)
mesh = meshing.generate_blockmesh(time, core.dictionary.read(f"{CASE}/system/blockMeshDict"))

# %%
# Define a uniform line
# ---------------------
# A ``meshSearch`` is required once per mesh; pass it into every
# ``sampledSet.New`` call.

from pybFoam import Word
from pybFoam.sampling import UniformSetConfig, meshSearch, sampledSet

search = meshSearch(mesh)
cfg = UniformSetConfig(
    axis="distance",
    start=[0.1, 0.5, 0.005],
    end=[0.9, 0.5, 0.005],
    nPoints=50,
)
line = sampledSet.New(Word("xLine"), mesh, search, cfg.to_foam_dict())

print(f"points       : {line.nPoints()}")
print(f"axis         : {line.axis()}")

# %%
# Inspect the line geometry
# -------------------------

import numpy as np

points = np.asarray(line.points())
distance = np.asarray(line.distance())
cells = line.cells()

print(f"points.shape   : {points.shape}")
print(f"distance[0..3] : {distance[:3]}")
print(f"valid cells    : {sum(1 for c in cells if c >= 0)}/{len(cells)}")

# %%
# Interpolate a scalar field on the set
# -------------------------------------

from pybFoam import volScalarField
from pybFoam.sampling import interpolationScalar, sampleSetScalar

p_rgh = volScalarField.read_field(mesh, "p_rgh")

interp = interpolationScalar.New(Word("cellPoint"), p_rgh)
sampled = sampleSetScalar(line, interp)

values = np.asarray(sampled)
valid = values[values < 1e10]  # sentinel for points outside the mesh
print(f"p_rgh along xLine — min={valid.min():.3e}  max={valid.max():.3e}")

# %%
# Reuse the same set for a vector field
# -------------------------------------
# A ``sampledSet`` is independent of the field — build it once, then
# drive multiple interpolators against it.

from pybFoam import volVectorField
from pybFoam.sampling import interpolationVector, sampleSetVector

U = volVectorField.read_field(mesh, "U")

interp_U = interpolationVector.New(Word("cellPoint"), U)
sampled_U = sampleSetVector(line, interp_U)

print(f"U along xLine shape: {np.asarray(sampled_U).shape}")
