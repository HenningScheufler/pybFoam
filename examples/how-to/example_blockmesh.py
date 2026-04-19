"""
Run blockMesh and checkMesh from Python
=======================================

This example builds a mesh from ``system/blockMeshDict`` with
:func:`pybFoam.meshing.generate_blockmesh` and then inspects it with
:func:`pybFoam.meshing.checkMesh`. Both are direct bindings of the
OpenFOAM utilities — the returned ``fvMesh`` is a regular mesh that can
be used immediately for field access, sampling, or further checks.
"""

# %%
# Prepare a working case
# ----------------------
# The ``examples/`` tree is the documentation baseline and must stay
# pristine — clone the case into a tmp directory before touching it.
# The meshing entry points then read the case through an ``argList``.

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
CASE = Path(tempfile.mkdtemp(prefix="pybfoam_blockmesh_")) / "case"
shutil.copytree(BASELINE, CASE)
print(f"working case: {CASE}")

args = core.argList([str(CASE), "-case", str(CASE)])
time = core.Time(args)

# %%
# Generate the mesh
# -----------------

block_mesh_dict = core.dictionary.read(f"{CASE}/system/blockMeshDict")
mesh = meshing.generate_blockmesh(time, block_mesh_dict)

print(f"nCells  = {mesh.nCells()}")
print(f"nFaces  = {mesh.nFaces()}")
print(f"nPoints = {mesh.nPoints()}")

# %%
# Inspect mesh quality
# --------------------
# ``checkMesh`` returns a structured dict with ``mesh_stats``,
# ``cell_types``, ``topology``, ``geometry``, ``quality``, ``passed``,
# ``failed``, and ``total_errors``. Select the flags you want — here we
# run the topology + geometry checks.

result = meshing.checkMesh(
    mesh,
    check_topology=True,
    all_topology=True,
    all_geometry=True,
    check_quality=False,
)

print(f"passed       = {result['passed']}")
print(f"total_errors = {result['total_errors']}")
print(f"hex cells    = {result['cell_types'].get('hexahedra', 0)}")
print(f"max non-orth = {result['geometry']['max_non_orthogonality']:.3f}")

# %%
# The full structure (top-level keys)
# -----------------------------------

for key in result:
    print(key)
