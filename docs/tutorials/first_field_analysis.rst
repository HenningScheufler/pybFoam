First field analysis
====================

This tutorial extends :doc:`getting_started`: you will loop over the time
directories of a completed OpenFOAM run, compute a summary statistic for
each, and plot the trace.

Prerequisite
------------

A case with more than one time directory on disk — for example, the cavity
tutorial run from an OpenFOAM installation after ``icoFoam`` has produced
``0.05``, ``0.1``, … in addition to ``0/``.

.. code-block:: bash

   cp -r $FOAM_TUTORIALS/incompressible/icoFoam/cavity/cavity ./cavity
   cd cavity
   blockMesh
   icoFoam

Step 1 — Enumerate the time steps
---------------------------------

``pybFoam.selectTimes`` wraps OpenFOAM's ``timeSelector``. Combined with
``Time.times()`` it gives you the list of directories actually on disk:

.. code-block:: python

   import pybFoam as pf

   time  = pf.Time(".", ".")
   steps = list(time.times())
   print([t.name() for t in steps])

Each element is an ``instant`` with ``.name()`` (directory name) and
``.value()`` (float time).

Step 2 — Read a field per step and summarise
--------------------------------------------

.. code-block:: python

   import numpy as np
   import pybFoam as pf

   time = pf.Time(".", ".")
   mesh = pf.fvMesh(time)

   rows = []
   for t in time.times():
       time.setTime(t, 0)       # advance to that step
       mesh.readUpdate()        # refresh if the mesh changed
       U = pf.volVectorField.read_field(mesh, "U")
       U_mag = np.linalg.norm(np.asarray(U["internalField"]), axis=1)
       rows.append((t.value(), U_mag.mean(), U_mag.max()))

   for t, umean, umax in rows:
       print(f"t={t:7.3f}  |U|_mean={umean:7.4f}  |U|_max={umax:7.4f}")

The ``np.asarray`` view does **not** copy — each iteration uses the same
buffer OpenFOAM manages. That keeps the loop cheap even for large meshes.

Step 3 — Plot it
----------------

With ``matplotlib`` installed (``pip install matplotlib``):

.. code-block:: python

   import matplotlib.pyplot as plt

   ts, umean, umax = zip(*rows)
   plt.plot(ts, umean, label="mean |U|")
   plt.plot(ts, umax,  label="max |U|")
   plt.xlabel("time [s]")
   plt.ylabel("|U| [m/s]")
   plt.legend()
   plt.show()

Step 4 — Aside: live analysis during a solver run
-------------------------------------------------

The exact same loop body runs from inside a solver if you are driving the
time loop yourself (see ``tests/cavity/icoFoam.py``). The difference is
that ``time.loop()`` advances the clock and ``runTime.write()`` commits a
step — otherwise the field code is identical.

Continue from here
------------------

* :doc:`first_sampling_workflow` — restrict the analysis to a plane or a
  line instead of the whole volume.
* :doc:`../how-to/compute_fvc_fvm` — derive new fields from the ones you
  read (``grad``, ``div``, ``laplacian``).
