The sampling config layer
=========================

Surface and set sampling in pybFoam is driven from Python through a small
Pydantic layer: ``pybFoam.sampling`` ships one config model per surface and
set kind — ``SampledPlaneConfig``, ``SampledIsoSurfaceConfig``,
``UniformSetConfig``, and so on. Each config has a ``to_foam_dict()``
method that produces the native OpenFOAM ``dictionary`` the C++ factories
expect. This page explains why that layer exists.

The alternative — and why it wasn't enough
------------------------------------------

OpenFOAM's ``sampledSurface::New(name, mesh, dict)`` and the equivalent
``sampledSet::New`` take a raw dictionary. pybFoam could have stopped at
the raw binding and asked users to build the dictionary themselves with
``pybFoam.dictionary``. That works, but:

* Every surface and set kind has a different set of keys. The reader has
  to consult the OpenFOAM source to find out what a valid
  ``distanceSurface`` dictionary looks like.
* Typos in keys are runtime errors, often silent ones — the surface will
  construct with defaults instead of failing.
* Nothing guides IDE autocompletion; nothing validates types.

The typed layer on top
----------------------

A config model is a one-to-one mapping of the keys OpenFOAM expects for
that surface/set kind, plus Pydantic validation:

.. code-block:: python

   from pybFoam.sampling import SampledPlaneConfig

   cfg = SampledPlaneConfig(
       point=[0.5, 0.5, 0.0],
       normal=[1.0, 0.0, 0.0],
   )
   cfg.to_foam_dict()   # native OpenFOAM dictionary, ready for .New()

The C++ factory methods still accept a ``dictionary`` — the typed config
layer is strictly Python-side. That keeps the binding surface small
(one ``New()`` overload per factory, not one per surface kind) while
giving callers typed constructors, IDE autocompletion, and per-field
validation for the full set of surface and set kinds OpenFOAM supports.

Where not to use it
-------------------

The config layer is a *user-facing convenience*. Inside a tight loop —
for example, regenerating a surface every step — build the dictionary
once and reuse the resulting surface object; there is no benefit to
re-validating the config each iteration. The layer exists to replace
ad-hoc dictionary construction at call sites, not to replace OpenFOAM's
dictionary for performance-sensitive work.
