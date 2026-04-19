Sample fields on surfaces and lines
===================================

The ``pybFoam.sampling`` module wraps OpenFOAM's ``sampledSurface`` and
``sampledSet`` families. All surface/set types are driven by a Pydantic
config model that converts to a native OpenFOAM ``dictionary`` via
``to_foam_dict()`` and is then handed to a ``.New()`` factory.

Available surface configs
-------------------------

Imported from ``pybFoam.sampling``:

* ``SampledPlaneConfig`` — arbitrary plane defined by point + normal.
* ``SampledPatchConfig`` — existing boundary patches.
* ``SampledCuttingPlaneConfig`` — mesh cut by a plane.
* ``SampledIsoSurfaceConfig`` — iso-surface of a scalar field.
* ``SampledMeshedSurfaceConfig``, ``SampledDistanceSurfaceConfig``,
  ``SampledFaceZoneConfig``, ``SampledThresholdCellFacesConfig``, etc.

Available set (line/cloud) configs
----------------------------------

* ``UniformSetConfig`` — uniformly spaced line between two points.
* ``PolyLineSetConfig`` — multi-segment line.
* ``CloudSetConfig`` — arbitrary cloud of points.
* ``CircleSetConfig`` — circle in a plane.
* ``PatchSeedSetConfig``, ``PatchCloudSetConfig``, ``ArraySetConfig``, etc.

Interpolate a scalar field on a plane
-------------------------------------

.. code-block:: python

   from pybFoam import Time, Word, fvMesh, volScalarField
   from pybFoam.sampling import (
       SampledPlaneConfig,
       interpolationScalar,
       sampledSurface,
       sampleOnFacesScalar,
   )

   time  = Time(".", ".")
   mesh  = fvMesh(time)
   p     = volScalarField.read_field(mesh, "p_rgh")

   plane = sampledSurface.New(
       Word("myPlane"),
       mesh,
       SampledPlaneConfig(point=[0.5, 0.5, 0.0], normal=[1.0, 0.0, 0.0]).to_foam_dict(),
   )
   plane.update()   # needs to be called before accessing geometry

   interp  = interpolationScalar.New(Word("cellPoint"), p)
   sampled = sampleOnFacesScalar(plane, interp)   # len(sampled) == len(plane.magSf())

   import numpy as np
   values = np.asarray(sampled)
   centers = np.asarray(plane.Cf())

Interpolation schemes: ``"cell"``, ``"cellPoint"``, ``"cellPointFace"``.
Use ``sampleOnPointsScalar`` instead if you want values at the surface's
points rather than face centers.

Interpolate a vector field
--------------------------

.. code-block:: python

   from pybFoam import volVectorField
   from pybFoam.sampling import interpolationVector, sampleOnFacesVector

   U       = volVectorField.read_field(mesh, "U")
   interp  = interpolationVector.New(Word("cellPoint"), U)
   sampled = sampleOnFacesVector(plane, interp)

   np.asarray(sampled).shape  # (n_faces, 3)

Sample along a uniform line
---------------------------

.. code-block:: python

   from pybFoam.sampling import (
       UniformSetConfig,
       meshSearch,
       sampledSet,
       sampleSetScalar,
   )

   search = meshSearch(mesh)
   line = sampledSet.New(
       Word("xLine"),
       mesh,
       search,
       UniformSetConfig(
           axis="distance",
           start=[0.1, 0.5, 0.005],
           end=[0.55, 0.5, 0.005],
           nPoints=50,
       ).to_foam_dict(),
   )

   interp  = interpolationScalar.New(Word("cellPoint"), p)
   sampled = sampleSetScalar(line, interp)

   points   = np.asarray(line.points())
   distance = np.asarray(line.distance())   # monotonic along the line
   cells    = line.cells()                  # -1 for points outside mesh

Points that fall outside the mesh are either dropped (``sampledSet`` reduces
``nPoints()`` automatically) or returned with a sentinel value ≥ 1e10 in
the sample array. Mask them before analysis:

.. code-block:: python

   arr = np.asarray(sampled)
   valid = arr[arr < 1e10]

Reuse a single set for multiple fields
--------------------------------------

A ``sampledSet`` / ``sampledSurface`` is independent of the field — create
it once and drive multiple ``interpolation*`` objects against it.

.. code-block:: python

   interp_p = interpolationScalar.New(Word("cellPoint"), p)
   interp_U = interpolationVector.New(Word("cellPoint"), U)

   sampled_p = sampleSetScalar(line, interp_p)
   sampled_U = sampleSetVector(line, interp_U)

See ``tests/pybind/test_set_sampling.py`` and ``test_surface_sampling.py``
for worked examples of every set and surface type.
