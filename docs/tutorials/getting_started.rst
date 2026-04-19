Getting started
===============

This tutorial walks through installing pybFoam, opening an OpenFOAM case from
Python, and reading a field. It takes about ten minutes and is the
prerequisite for the other tutorials.

Before you begin
----------------

You need:

* OpenFOAM v2312 or newer, installed and its environment sourced.
* Python ≥ 3.9 (a ``venv`` or ``uv`` project is recommended).

Step 1 — Source OpenFOAM
------------------------

Every pybFoam session — including installation — needs a shell where
OpenFOAM's environment is active:

.. code-block:: bash

   source /path/to/OpenFOAM-v2506/etc/bashrc

Verify that the relevant variables are set:

.. code-block:: bash

   echo "$FOAM_SRC $FOAM_LIBBIN $WM_LABEL_SIZE $FOAM_API"

If any are empty, the build will fail — see
:doc:`../reference/build_system`.

Step 2 — Install pybFoam
------------------------

Inside your virtual environment:

.. code-block:: bash

   pip install pybFoam[all]

Or for a development checkout:

.. code-block:: bash

   git clone https://github.com/HenningScheufler/pybFoam
   cd pybFoam
   pip install -e .[all]

Step 3 — Open a case
--------------------

pybFoam ships a minimal case under ``examples/case`` to experiment with.
From a shell with OpenFOAM sourced:

.. code-block:: bash

   cd examples/case
   ./Allrun      # runs blockMesh — no solver

Now open a Python session in that directory:

.. code-block:: python

   import pybFoam as pf

   time = pf.Time(".", ".")
   mesh = pf.fvMesh(time)

   print(f"nCells   = {mesh.nCells()}")
   print(f"nPoints  = {mesh.nPoints()}")
   print(f"nFaces   = {mesh.nFaces()}")

* ``pf.Time(".", ".")`` is the two-argument form that treats the current
  directory as both the root and the case — convenient for interactive
  work.
* ``pf.fvMesh(time)`` reads ``constant/polyMesh`` and returns a working mesh.

Step 4 — Read a field
---------------------

Any field listed under ``0/`` (or ``0.orig/``) can be read directly:

.. code-block:: python

   import numpy as np

   p_rgh = pf.volScalarField.read_field(mesh, "p_rgh")
   U     = pf.volVectorField.read_field(mesh, "U")

   # Zero-copy NumPy views
   p_array = np.asarray(p_rgh["internalField"])
   U_array = np.asarray(U["internalField"])

   print(f"Pressure: {p_array.min():.3f} … {p_array.max():.3f}")
   print(f"Velocity has shape {U_array.shape}")

The ``["internalField"]`` indexer returns the cell-centred field; boundary
values live under their patch names, e.g. ``U["movingWall"]``.

Where to go next
----------------

* :doc:`first_field_analysis` — loop over time steps and summarise fields.
* :doc:`first_sampling_workflow` — extract values along a plane and export
  them to CSV.
* :doc:`../how-to/compute_fvc_fvm` — compute gradients, divergences, and
  Laplacians.
