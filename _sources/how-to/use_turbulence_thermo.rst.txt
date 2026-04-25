Access turbulence and thermophysical models
===========================================

pybFoam binds the OpenFOAM turbulence and thermophysical libraries as the
``pybFoam.turbulence`` and ``pybFoam.thermo`` submodules. They expose the
factory methods and model interfaces — creation and use mirrors the C++
patterns (``turbulenceModel::New(U, phi, ...)``,
``basicThermo::New(mesh)``).

.. note::
   The full API is module-generated; see :doc:`../reference/api` for the
   exhaustive list. This page sketches the common entry points. When in
   doubt, consult the ``turbulence`` / ``thermo`` sections in the
   :doc:`API reference <../reference/api>` and the corresponding C++
   documentation — the Python names track the C++ ones.

Turbulence models (incompressible)
----------------------------------

Create an incompressible turbulence model bound to your velocity and flux
fields. The model then exposes ``nut()``, ``k()``, ``epsilon()``, ``omega()``,
``divDevReff()``, and ``correct()`` depending on the concrete model type
selected in ``constant/turbulenceProperties``.

.. code-block:: python

   from pybFoam import Time, fvMesh, createPhi, volScalarField, volVectorField
   from pybFoam import turbulence

   time = Time(".", ".")
   mesh = fvMesh(time)
   U    = volVectorField.read_field(mesh, "U")
   phi  = createPhi(U)
   nu   = volScalarField.read_field(mesh, "nu")

   model = turbulence.incompressible.turbulenceModel.New(U, phi, nu)

   # After solving U, update eddy viscosity
   model.correct()

   nut = model.nut()   # volScalarField

The equivalent compressible entry point lives in
``turbulence.compressible.turbulenceModel``.

Thermophysical models
---------------------

Create a fluid thermo model against the mesh (reads
``constant/thermophysicalProperties``):

.. code-block:: python

   from pybFoam import thermo

   thermoModel = thermo.fluidThermo.New(mesh)

   thermoModel.correct()
   T     = thermoModel.T()       # volScalarField
   rho   = thermoModel.rho()
   alpha = thermoModel.alpha()   # laminar thermal diffusivity

``thermo.solidThermo`` follows the same pattern for solid regions.

Putting it together in a solver
-------------------------------

In a PISO/PIMPLE-style solver you typically hold the turbulence model
alongside ``U``/``phi`` and call ``.correct()`` after each pressure-velocity
loop. See ``tests/cavity/icoFoam.py`` for the incompressible PISO loop —
add a ``turbulence`` model and ``model.correct()`` at the end of each outer
iteration to turn it into a turbulent simulation.
