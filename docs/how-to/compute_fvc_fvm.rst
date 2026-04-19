Compute finite-volume operators (fvc, fvm)
==========================================

pybFoam exposes OpenFOAM's finite-volume operator machinery through two
submodules that mirror the C++ API:

* ``pybFoam.fvc`` — **explicit** operators. Each call returns a
  ``tmp_*`` field wrapper; call the wrapper to materialise it.
* ``pybFoam.fvm`` — **implicit** operators. Each call returns a
  matrix term (``fvScalarMatrix``/``fvVectorMatrix``) that can be summed
  into an equation and solved.

Explicit operators (``fvc``)
----------------------------

.. code-block:: python

   from pybFoam import Time, fvMesh, fvc, volScalarField, volVectorField, createPhi

   time  = Time(".", ".")
   mesh  = fvMesh(time)
   p_rgh = volScalarField.read_field(mesh, "p_rgh")
   U     = volVectorField.read_field(mesh, "U")
   phi   = createPhi(U)

   grad_p = fvc.grad(p_rgh)()    # tmp_volVectorField -> volVectorField
   div_U  = fvc.div(U)()
   lap_p  = fvc.laplacian(p_rgh)()

   flux         = fvc.flux(U)()              # surfaceScalarField
   div_phiU     = fvc.div(flux, U)()
   div_phigradP = fvc.div(flux, fvc.grad(p_rgh))()

   snGrad_p     = fvc.snGrad(p_rgh)()        # surface-normal gradient
   reconstructed = fvc.reconstruct(flux)()   # scalar flux -> vector field

Note the trailing ``()``: most operators return a ``tmp_*`` reference to let
OpenFOAM reuse the underlying buffer. Calling it returns the concrete field.
If you pass the ``tmp_*`` directly into another ``fvc`` / ``fvm`` call, the
conversion happens automatically — only materialise when you actually want
the final value.

Implicit/matrix operators (``fvm``)
-----------------------------------

``fvm`` operators produce terms of an equation matrix rather than field
values. Combine them with ``+`` / ``-`` and wrap into an ``fvScalarMatrix``
or ``fvVectorMatrix`` before solving.

.. code-block:: python

   from pybFoam import fvm, fvScalarMatrix, fvVectorMatrix

   # scalar Laplacian: a single-term pressure Poisson matrix
   pEqn = fvScalarMatrix(fvm.laplacian(p_rgh))

   # vector momentum: ddt + convection - diffusion
   nu = volScalarField.read_field(mesh, "nu")
   UEqn = fvVectorMatrix(
       fvm.ddt(U)
       + fvm.div(phi, U)
       - fvm.laplacian(nu, U)
   )

Mixing explicit and implicit terms
----------------------------------

A typical PISO/SIMPLE loop combines both submodules: the momentum matrix is
built with ``fvm``, the pressure gradient is added as an explicit ``fvc``
term, and the system is handed to ``solve``:

.. code-block:: python

   from pybFoam import solve

   if piso.momentumPredictor():
       solve(UEqn + fvc.grad(p))

See ``tests/cavity/icoFoam.py`` for a complete PISO solver implementation
that exercises the full ``fvc``/``fvm`` surface together with ``pisoControl``,
``setRefCell``, ``constrainHbyA``, and ``constrainPressure``.
