Code organization
=================

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

Every binding module is built with ``NB_SHARED``, so the nanobind
type registry lives in a separate ``libnanobind.so`` installed
alongside the modules (located at load time via ``RPATH=$ORIGIN``).
All of pybFoam's submodules share that one registry.

A downstream nanobind extension that wants to take pybFoam types as
parameters has to opt in to the same registry — it isn't automatic.
The consumer must build with ``NB_SHARED`` against the same nanobind
SONAME, RPATH-link to pybFoam's installed ``libnanobind.so``, **and**
ensure ``pybFoam`` is imported before any of its types are looked up
(``import pybFoam`` from Python, or
``nb::module_::import_("pybFoam")`` from an embedded interpreter).
``pyOFTools`` is the worked example — see its
``cmake/Dependencies.cmake`` and ``embeddingPython/pyFunctionObject.cpp``.


The embed library
-----------------

``pybFoam/embed/`` is a separate C++ shared library
(``libpybFoamEmbed.so``) built when ``PYBFOAM_BUILD_EMBED=ON`` (the
default). It is designed to be consumed **from the OpenFOAM side** —
OpenFOAM solvers link against it via a CMake package config to embed a
Python interpreter. The install layout and CMake targets are asserted in
the ``verify-embed-install`` CI job.
