Read and write OpenFOAM dictionaries
====================================

pybFoam ships two complementary layers for dictionary I/O:

1. **Raw dictionaries** via ``pybFoam.dictionary`` — a thin binding of
   OpenFOAM's ``dictionary`` with a templated ``get[T]()`` accessor.
2. **Typed models** via ``pybFoam.io.IOModelBase`` — a Pydantic mixin that
   validates, round-trips to/from OpenFOAM dict, YAML, or JSON, and is
   extensible with ``create_model``.

Raw dictionary access
---------------------

Read a dictionary from disk and pull typed values out of it.

.. code-block:: python

   from pybFoam import Word, dictionary, tensor, vector

   d = dictionary.read("system/controlDict")

   application = d.get[Word]("application")
   end_time    = d.get[float]("endTime")
   write_step  = d.get[int]("writeInterval")

   # Supply a default when the key may be absent
   max_co = d.getOrDefault[float]("maxCo", 0.5)

   # Nested subdictionaries
   piso = d.subDict("PISO")
   n_correctors = piso.get[int]("nCorrectors")

   # List of top-level keys
   for name in d.toc():
       print(name)

Typed dictionary fields (``scalarField``, ``vectorField``, ``tensorField``,
``wordList``) are returned by the same ``get[T]()`` interface:

.. code-block:: python

   import numpy as np
   import pybFoam

   d = dictionary.read("TestDict")
   scalars = d.get[pybFoam.scalarField]("scalarField")   # numpy-viewable
   vectors = d.get[pybFoam.vectorField]("vectorField")
   words   = d.get[pybFoam.wordList]("wordList").list()  # -> list[str]

   np.asarray(scalars)  # zero-copy

Typed models with ``IOModelBase``
---------------------------------

Subclass ``IOModelBase`` to declare the fields you expect:

.. code-block:: python

   from pydantic import Field
   from pybFoam import Word, tensor, vector
   from pybFoam.io.model_base import IOModelBase

   class TransportProperties(IOModelBase):
       nu: float = Field(..., description="Kinematic viscosity")

       class Config:
           foam_file_name = "transportProperties"

   props = TransportProperties.from_file("constant/transportProperties")
   print(props.nu)
   props.nu = 1.5e-5
   props.to_file("constant/transportProperties")

``from_file`` auto-detects the format from the suffix — OpenFOAM dict by
default, ``.yaml`` / ``.json`` otherwise:

.. code-block:: python

   TransportProperties.from_file("props.yaml")  # YAML input
   TransportProperties.from_file("props.json")  # JSON input

Reusing the built-in ``controlDict`` / ``fvSchemes`` / ``fvSolution`` bases
---------------------------------------------------------------------------

``pybFoam.io.system`` ships ready-made base classes that cover every key
OpenFOAM writes out of the box. Extend them with ``pydantic.create_model``
to add the case-specific entries you need.

.. code-block:: python

   from pydantic import Field, create_model
   from pybFoam.io.system import ControlDictBase

   ControlDict = create_model(
       "ControlDict",
       maxCo=(float, Field(..., gt=0.0)),
       test_token=(str, Field(...)),
       __base__=ControlDictBase,
   )

   cd = ControlDict.from_file("system/controlDict")
   assert cd.application == "icoFoam"
   assert cd.maxCo == 0.5

The same pattern works for ``FvSchemesBase`` and ``FvSolutionBase`` — see
``tests/io/`` for worked examples including extending ``DIVSchemes`` with
aliased keys like ``div(phi,U)``.

Nested submodels
----------------

Nested subdictionaries map to nested ``IOModelBase`` subclasses:

.. code-block:: python

   from pybFoam.io.model_base import IOModelBase
   from pybFoam import Word

   class Sub(IOModelBase):
       word2: Word

   class Top(IOModelBase):
       word: Word
       sub: Sub

   t = Top.from_file("TestDict")
   t.sub.word2
