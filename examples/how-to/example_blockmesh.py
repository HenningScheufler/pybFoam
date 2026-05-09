"""
Run blockMesh and checkMesh from Python
=======================================

Build a mesh from ``system/blockMeshDict`` with
:func:`pybFoam.meshing.generate_blockmesh` and inspect it with
:func:`pybFoam.meshing.checkMesh`. Both are direct bindings of the
OpenFOAM utilities — the returned ``fvMesh`` is a regular mesh that can
be used immediately for field access, sampling, or further checks.

Prerequisite: OpenFOAM v2312+ sourced; pybFoam installed.
"""

# %%
# Prepare a working case
# ----------------------
# :func:`pybFoam.clone_case` copies the named example into a tmp
# directory so the baseline under ``examples/`` stays pristine.

from pybFoam import Time, argList, clone_case, dictionary
from pybFoam.meshing import checkMesh, generate_blockmesh

case = clone_case("case")
print(f"working case: {case}")

# %%
# Generate the mesh
# -----------------

time = Time(argList([str(case), "-case", str(case)]))
mesh = generate_blockmesh(time, dictionary.read(str(case / "system" / "blockMeshDict")))

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

result = checkMesh(
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
# Patch breakdown
# ---------------
# Iterate ``mesh.boundary()`` (the ``fvBoundaryMesh``) to see what
# patches the mesh actually has — useful when verifying that the
# boundary section of ``blockMeshDict`` came through as expected.

boundary = mesh.boundary()
patches = []
for i in range(len(boundary)):
    patch = boundary[i]
    name = str(patch.name())
    size = patch.size()
    patches.append((name, size))
    print(f"{name:<16s} faces={size}")

# %%
# Plot the patch face counts
# --------------------------
# A simple matplotlib bar chart confirms the mesh has the patches the
# user expects. A missing or zero-face patch shows up immediately.

import matplotlib.pyplot as plt

names = [p[0] for p in patches]
sizes = [p[1] for p in patches]

fig, ax = plt.subplots(figsize=(6, 3))
ax.bar(names, sizes, color="steelblue")
ax.set_ylabel("number of faces")
ax.set_title(f"Boundary patches ({mesh.nCells()} internal cells)")
for i, n in enumerate(sizes):
    ax.text(i, n, f"{n}", ha="center", va="bottom", fontsize=9)
fig.tight_layout()
plt.show()

# %%
# See also
# --------
#
# - :doc:`example_read_dictionaries` — extract typed field entries from
#   any dictionary, including ``blockMeshDict``.
