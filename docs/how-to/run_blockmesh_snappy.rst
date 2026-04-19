Generate meshes with blockMesh and snappyHexMesh
================================================

The ``pybFoam.meshing`` submodule exposes three entry points:

* ``generate_blockmesh(time, dict)`` — equivalent to the ``blockMesh`` utility.
* ``generate_snappyhexmesh(...)`` — equivalent to ``snappyHexMesh``.
* ``checkMesh(mesh, **flags)`` — equivalent to ``checkMesh``, returning a
  structured dict instead of parsing stdout.

Run blockMesh on an existing case
---------------------------------

.. code-block:: python

   import pybFoam.pybFoam_core as core
   from pybFoam import meshing

   case = "/path/to/case"
   args = core.argList([case, "-case", case])
   time = core.Time(args)

   block_mesh_dict = core.dictionary.read(f"{case}/system/blockMeshDict")
   mesh = meshing.generate_blockmesh(time, block_mesh_dict)

The returned ``mesh`` is a regular ``fvMesh`` and can be used immediately for
field access, sampling, or further checks.

Run checkMesh and inspect the result
------------------------------------

.. code-block:: python

   result = meshing.checkMesh(
       mesh,
       check_topology=True,
       all_topology=True,
       all_geometry=True,
       check_quality=False,
   )

   assert result["passed"] is True
   print(result["mesh_stats"]["cells"])           # int
   print(result["geometry"]["max_non_orthogonality"])
   print(result["cell_types"]["hexahedra"])

The full result dict has these top-level keys: ``mesh_stats``, ``cell_types``,
``topology``, ``geometry``, ``quality``, ``passed``, ``failed``,
``total_errors``. See ``tests/meshing/test_checkmesh.py`` for the exhaustive
list of fields.

Compare a Python-built mesh against the native tool
---------------------------------------------------

``tests/meshing/test_blockmesh_cube.py`` is a ready-made template for a
regression test: it runs the native ``blockMesh`` binary in one tmp
directory, the Python binding in another, and asserts identical cell/face
/point counts. The same pattern works for ``snappyHexMesh``.

snappyHexMesh
-------------

``generate_snappyhexmesh`` takes a base ``fvMesh`` (usually produced by
``generate_blockmesh``) plus the ``snappyHexMeshDict`` and surface feature
data. See ``tests/meshing/test_snappy_motorbike.py`` and
``test_snappy_sphere.py`` for complete cases.
