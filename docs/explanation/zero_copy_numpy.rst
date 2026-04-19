Zero-copy NumPy access
======================

pybFoam's ``Field<T>`` bindings expose the underlying OpenFOAM storage via
the Python buffer protocol. ``np.asarray(field)`` constructs a NumPy view
that *shares memory* with OpenFOAM's internal buffer — no copy, no
conversion. This page explains what that means for correctness and
lifetime.

The contract
------------

For a ``scalarField`` the buffer is a contiguous 1-D array of
``Foam::scalar`` (typically ``double``). For ``vectorField``, a 2-D
``(N, 3)`` array. For ``tensorField``, a 2-D ``(N, 9)`` array. The buffer
is writable — assigning through the NumPy view mutates the OpenFOAM field.

.. code-block:: python

   from pybFoam import scalarField
   import numpy as np

   a = scalarField([0.0] * 10)
   b = scalarField([1.0] * 10)

   np_a = np.asarray(a)
   np_b = np.asarray(b)

   a += b + 5.0             # mutation via the C++ binding
   assert np_a[0] == 6.0    # visible through the NumPy view

   np_a[0:2] = 42.0         # mutation via NumPy
   assert a[0] == 42.0      # visible through the C++ binding

The same contract holds for ``volScalarField.internalField()`` and for the
geometry accessors of sampling objects (``plane.Cf()``, ``plane.points()``,
``line.distance()``).

Why this matters
----------------

OpenFOAM's real fields can have 10⁶ – 10⁹ elements. Copying them on every
Python access would make pybFoam unusable for the workflows it exists to
enable — summary statistics, masking, plotting, feeding an ML model, or
writing a custom solver loop. Zero-copy access is a load-bearing feature,
not a convenience.

Lifetime — who owns the buffer
------------------------------

The buffer is owned by the OpenFOAM object, not by the NumPy view. If the
owning object is destroyed, the NumPy view becomes a dangling pointer.
nanobind installs a keep-alive relationship from the NumPy array back to
the binding object so that this is safe **as long as you don't outlive the
C++ owner another way** — for example:

* Don't stash ``np.asarray(field["internalField"])`` in a dict while
  letting ``field`` itself go out of scope in the same frame.
* Don't pass a view across a ``runTime.loop()`` boundary if the field is
  read fresh each step.
* When slicing a mesh's cell-centre array ``mesh.C()["internalField"]``,
  hold a reference to ``mesh`` (and therefore ``time``) for as long as you
  use the view — that's why the sampling tests do
  ``time, mesh = create_time_mesh()  # time must stay alive``.

The ``tmp_*`` wrapper types
---------------------------

OpenFOAM's C++ operators return ``tmp<Field<T>>`` — a reference-counted
wrapper that exists so the library can reuse buffers rather than allocate
a fresh one per operation. pybFoam mirrors this pattern directly: every
``fvc`` operator returns a ``tmp_volScalarField`` (or tensor/vector/surface
equivalent), and ``fvm`` operators return ``tmp_fvScalarMatrix`` and so on.

To get the concrete field, call the wrapper:

.. code-block:: python

   t = fvc.grad(p_rgh)     # tmp_volVectorField
   grad_p = t()             # volVectorField — materialised

Once materialised, the ``tmp_*`` is consumed — its internal pointer is
null. Don't call it twice, and don't hold on to a ``tmp_*`` long-term.
If you pass a ``tmp_*`` straight into another operator (``fvc.div(phi,
fvc.grad(p))``), the inner binding consumes it without you needing to
materialise manually.

When you *do* need a copy
-------------------------

If you genuinely need an independent buffer (e.g. to keep a snapshot
across a time step that overwrites the field), ``np.asarray(field).copy()``
makes a fresh NumPy array that no longer aliases OpenFOAM's storage.

.. code-block:: python

   snapshot = np.asarray(U["internalField"]).copy()
   # safe to use after the next runTime.loop() even if U is re-read
