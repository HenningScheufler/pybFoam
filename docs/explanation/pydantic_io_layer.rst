The Pydantic I/O layer
======================

Two submodules in pybFoam are written in Python rather than bound C++:
``pybFoam.io`` for dictionary models and ``pybFoam.sampling`` for surface
and set configs. Both are built on Pydantic. This page explains why.

The alternative — and why it wasn't enough
------------------------------------------

OpenFOAM's native dictionary format is one of pybFoam's raw-binding
surface areas: ``pybFoam.dictionary.read(...)`` returns a dictionary you
can probe with ``d.get[T]("key")``. That is flexible, fast, and covers any
dictionary OpenFOAM can parse — but:

* Keys are opaque strings. Typos are runtime errors, often silent ones:
  ``d.get[float]("endTme")`` throws at the point of use, potentially deep
  in a user script.
* There is no schema. A reviewer looking at your code cannot tell what the
  dictionary is *supposed* to contain without reading the source files.
* Conversion to/from other formats (YAML, JSON, Python objects) has to be
  written by hand, repeatedly.

The typed layer on top
----------------------

``IOModelBase`` is a mixin that adds four capabilities to a Pydantic model:

1. ``from_file(path)`` that parses OpenFOAM dict, YAML, or JSON based on
   the suffix.
2. ``to_file(path)`` that writes the same three formats.
3. Field validation via Pydantic's normal mechanisms — ``Field(..., gt=0)``,
   aliasing, nested submodels.
4. Field types that understand OpenFOAM primitives — ``Word``, ``vector``,
   ``tensor``, and the ``Field<T>`` family are valid type annotations, not
   only Python scalars.

The ready-made bases — ``ControlDictBase``, ``FvSchemesBase``,
``FvSolutionBase`` — cover the entries OpenFOAM writes out of the box.
Cases that need more are expected to extend them via ``pydantic.create_model``
rather than fork the base. The tests under ``tests/io/`` demonstrate this
extension pattern.

``to_foam_dict()`` in sampling configs
--------------------------------------

The ``pybFoam.sampling`` configs solve the same problem for sampling
surfaces and sets. Every config is a Pydantic model with a
``to_foam_dict()`` method that materialises an OpenFOAM ``dictionary``
ready to hand to ``sampledSurface.New`` or ``sampledSet.New``.

The C++ factory methods themselves accept a ``dictionary`` — the typed
config layer is strictly Python-side. This keeps the binding surface small
(one ``New()`` overload per factory, not one per surface type) while giving
users typed constructors, IDE autocompletion, and validation for the full
set of surface and set kinds OpenFOAM supports.

Where not to use it
-------------------

The Pydantic layer is a *user-facing convenience*. It is not appropriate
on the hot path — inside a tight solver loop, read the dictionary once,
convert to raw values, and then keep working with primitives. The layer
exists to replace hand-rolled parsing and ad-hoc string keys, not to
replace OpenFOAM's ``dictionary`` for performance-sensitive work.
