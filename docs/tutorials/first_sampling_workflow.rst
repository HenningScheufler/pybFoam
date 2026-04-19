First sampling workflow
=======================

This tutorial defines a sampling plane, interpolates a field onto it, and
exports the result as a CSV you can load in pandas or a spreadsheet. It
builds on :doc:`getting_started`.

Step 1 — Describe the sampling plane
------------------------------------

Sampling geometries in pybFoam are declared as Pydantic config models.
Writing the config in Python gives you type checking and IDE autocompletion,
and the ``.to_foam_dict()`` conversion keeps you from hand-writing the
dictionary string.

.. code-block:: python

   from pybFoam.sampling import SampledPlaneConfig

   config = SampledPlaneConfig(
       point=[0.05, 0.05, 0.005],
       normal=[1.0, 0.0, 0.0],
   )

Step 2 — Create the surface
---------------------------

.. code-block:: python

   from pybFoam import Time, Word, fvMesh
   from pybFoam.sampling import sampledSurface

   time = Time(".", ".")
   mesh = fvMesh(time)

   plane = sampledSurface.New(Word("midPlane"), mesh, config.to_foam_dict())
   plane.update()     # must be called before reading geometry

   print(f"faces: {len(plane.magSf())}   area: {plane.area():.4f}")

The returned ``plane`` exposes ``points()``, ``Cf()`` (face centres),
``Sf()`` (face area vectors), and ``magSf()`` (face areas).

Step 3 — Interpolate a scalar field
-----------------------------------

.. code-block:: python

   import numpy as np
   from pybFoam import volScalarField
   from pybFoam.sampling import interpolationScalar, sampleOnFacesScalar

   p = volScalarField.read_field(mesh, "p")

   interp  = interpolationScalar.New(Word("cellPoint"), p)
   sampled = sampleOnFacesScalar(plane, interp)

   centers = np.asarray(plane.Cf())
   values  = np.asarray(sampled)
   print(values.shape, centers.shape)   # (n_faces,), (n_faces, 3)

Choose ``"cell"`` for piecewise-constant (fastest), ``"cellPoint"`` for
continuous linear (good default), or ``"cellPointFace"`` for the most
accurate scheme at higher cost.

Step 4 — Export to CSV
----------------------

The Python standard library is enough; pandas works too if you prefer:

.. code-block:: python

   import csv

   with open("midPlane.csv", "w", newline="") as f:
       w = csv.writer(f)
       w.writerow(["x", "y", "z", "p"])
       for (x, y, z), v in zip(centers, values):
           w.writerow([x, y, z, v])

Or with pandas:

.. code-block:: python

   import pandas as pd

   df = pd.DataFrame(centers, columns=["x", "y", "z"])
   df["p"] = values
   df.to_csv("midPlane.csv", index=False)

Step 5 — Swap the geometry
--------------------------

Once the pattern is in your head, swap the config to sample different
geometries without touching the rest of the code:

.. code-block:: python

   from pybFoam.sampling import SampledPatchConfig, SampledIsoSurfaceConfig

   # A boundary patch:
   cfg = SampledPatchConfig(patches=["movingWall"])

   # An isosurface of p at 0.5:
   cfg = SampledIsoSurfaceConfig(isoField="p", isoValue=0.5)

Going further
-------------

* :doc:`../how-to/sample_surfaces_and_lines` — line samples, cloud samples,
  vector fields, sampling at points vs. faces.
* :doc:`../how-to/compute_fvc_fvm` — derive fields (gradient, divergence)
  and then sample the derived field.
* :doc:`../explanation/zero_copy_numpy` — the buffer-protocol contract that
  makes ``np.asarray(plane.Cf())`` cheap.
