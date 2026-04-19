Architecture
============

pybFoam is a Python package that ships several compiled nanobind extension
modules and a small pure-Python layer on top. This page explains how those
pieces fit together and why the package is split the way it is.

Module layout
-------------

At import time ``import pybFoam`` brings in:

.. code-block:: text

   pybFoam/
   ├── __init__.py            ← flat re-export from pybFoam_core
   ├── pybFoam_core.so        ← core bindings (Time, fvMesh, fields, dicts, …)
   ├── fvc.so                 ← explicit finite-volume operators
   ├── fvm.so                 ← implicit finite-volume operators (matrices)
   ├── meshing.so             ← blockMesh / snappyHexMesh / checkMesh
   ├── thermo.so              ← thermophysical models
   ├── turbulence.so          ← turbulence models
   ├── sampling_bindings.so   ← sampledSurface / sampledSet
   ├── runTimeTables.so       ← runtime table registry
   ├── libnanobind.so         ← shared nanobind runtime (see below)
   ├── io/                    ← pure-Python Pydantic I/O layer
   ├── sampling/              ← pure-Python Pydantic sampling configs
   └── embed/                 ← C++ embed library (for OpenFOAM solvers)

Each ``.so`` is a separate nanobind module compiled against the
``OpenFOAM::*`` CMake targets it needs — ``pybFoam_core`` links only
against ``finiteVolume``; ``meshing`` additionally links the mesh utilities;
``sampling`` against ``OpenFOAM::sampling``; and so on. The split mirrors
OpenFOAM's own library layout.

Why several modules and not one
-------------------------------

A single monolithic ``.so`` linking *every* OpenFOAM library would work, but
has two practical drawbacks:

1. **Link time dominates rebuilds.** Touching one binding would relink every
   OpenFOAM library — tens of seconds of wasted time in an edit loop.
2. **Users who only need part of the surface still pay.** Importing
   ``pybFoam.sampling`` would pull turbulence and thermo into the process
   unconditionally.

Splitting by subsystem keeps build feedback fast and keeps the import cost
proportional to what was asked for.

The shared ``libnanobind.so``
-----------------------------

Every binding module is built with ``NB_SHARED``, meaning the nanobind
type registry lives in a separate shared library (``libnanobind.so``)
that is installed alongside the modules and located at load time via
``RPATH=$ORIGIN``. Without this, each ``.so`` would embed its own copy of
the registry and C++ types defined in one module would not be recognised
by another — so a ``tmp_volScalarField`` returned from ``fvc`` could not
be consumed by ``fvm``.

This is a **private implementation detail of pybFoam**. Downstream projects
that write their own nanobind extensions (e.g. ``pyOFTools``) depend on
nanobind via their own ``pip install nanobind`` and rely on Linux loader
SONAME deduplication to share the same registry at runtime. The ABI flags
must match — both sides must be built with compatible nanobind releases.

The pure-Python layer
---------------------

Two sibling directories — ``pybFoam.io`` and ``pybFoam.sampling`` — are
written in Python:

* ``io/`` defines ``IOModelBase`` and the ready-made ``ControlDictBase`` /
  ``FvSchemesBase`` / ``FvSolutionBase`` models.
* ``sampling/`` defines Pydantic configs (``SampledPlaneConfig``,
  ``UniformSetConfig``, …) with ``.to_foam_dict()`` conversion.

Keeping these layers in Python means type checking, validation messages,
and extensibility (via ``pydantic.create_model``) happen in a well-tooled
environment, while the expensive work — mesh, fields, operators — stays in
the C++ bindings. See :doc:`pydantic_io_layer` for the rationale behind
this split.

The embed library
-----------------

``pybFoam/embed/`` is a separate C++ shared library
(``libpybFoamEmbed.so``) built when ``PYBFOAM_BUILD_EMBED=ON`` (the
default). It is designed to be consumed **from the OpenFOAM side** —
OpenFOAM solvers link against it via a CMake package config to embed a
Python interpreter. The install layout and CMake targets are asserted in
the ``verify-embed-install`` CI job.
