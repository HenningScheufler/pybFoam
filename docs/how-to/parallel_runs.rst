Run pybFoam in parallel
=======================

pybFoam scripts can run under MPI in the same way OpenFOAM solvers do: you
decompose the case, launch with ``mpirun``, and the usual
``Pstream`` facilities report your rank. The shipped ``Allrun-parallel``
scripts in ``tests/pybind/`` and ``examples/case/`` are the canonical
reference.

Decompose the case
------------------

Use OpenFOAM's ``decomposePar`` utility first. pybFoam does not re-wrap it —
invoke it the way you normally would:

.. code-block:: bash

   cd /path/to/case
   decomposePar -force

This creates ``processor0``, ``processor1``, … under the case directory.

Launch with MPI
---------------

.. code-block:: bash

   mpirun -np 4 python my_script.py -parallel

The ``-parallel`` flag is parsed by ``argList`` on each rank; without it
OpenFOAM assumes serial and you will hit immediate errors about parallel
I/O. Build ``argList`` from ``sys.argv`` as usual:

.. code-block:: python

   import sys
   import pybFoam
   from pybFoam import Time, fvMesh

   args = pybFoam.argList(sys.argv)
   time = Time(args)
   mesh = fvMesh(time)

Query parallel state from Python
--------------------------------

The ``Pstream`` class exposes the same predicates as the C++ side:

.. code-block:: python

   from pybFoam import Pstream, Info

   if Pstream.parRun():
       Info(f"Rank {Pstream.myProcNo()} of {Pstream.nProcs()}")

   # Only print from the master rank
   if Pstream.master():
       print("summary goes here")

In a serial run ``parRun()`` returns ``False``, ``myProcNo()`` is ``0``,
``nProcs()`` is ``1``, and ``master()`` is ``True`` — so the same script
works both ways.

Reconstruct afterwards
----------------------

Once the parallel run finishes, reassemble the case in the host directory
with ``reconstructPar``:

.. code-block:: bash

   reconstructPar -latestTime

This too is a native OpenFOAM utility and is not wrapped by pybFoam.
