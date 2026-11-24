NumPy integration
=================

pybFoam's ``Field<T>`` bindings make every field behave like a NumPy
array. ``np.asarray(field)`` constructs a view that *shares memory*
with OpenFOAM's internal buffer — no copy, no conversion. This page
describes the mechanism and the contract.

How it's done
-------------

NumPy's ``np.asarray(x)`` calls ``x.__array__()`` if the object has
one. Every ``Field<T>`` binding implements ``__array__`` as a small
lambda that hands NumPy a description of the underlying OpenFOAM
storage:

.. code-block:: cpp

   .def("__array__", [](Field<Type>& self, ...) {
       size_t shape[2] = { self.size(), pTraits<Type>::nComponents };
       return nb::ndarray<nb::numpy, scalar>(self.data(), ndim, shape);
   }, nb::rv_policy::reference_internal)

The ndarray is built directly over ``self.data()`` — only a
``(pointer, dtype, shape, strides)`` description crosses the language
boundary. Shape comes from ``pTraits<Type>::nComponents``: 1 for
``scalar``, 3 for ``vector``, 9 for ``tensor``, 6 for ``symmTensor``.
The ``reference_internal`` return policy keeps the field alive for as
long as the NumPy view exists.

The reverse direction — constructing a field from a NumPy array —
exists too (``scalarField(np_array)``). That one *copies* the data
into OpenFOAM-owned storage so the field can outlive the NumPy
buffer.

Using it
--------

A scalar field gives you a 1-D array; a multi-component field gives
you a 2-D ``(N, k)`` array:

.. list-table::
   :header-rows: 1

   * - Field type
     - ``np.asarray(field).shape``
     - dtype
   * - ``scalarField`` (size N)
     - ``(N,)``
     - ``float64`` (or ``float32`` on SP builds)
   * - ``vectorField`` (size N)
     - ``(N, 3)``
     - same as scalar
   * - ``tensorField`` (size N)
     - ``(N, 9)``
     - same as scalar
   * - ``symmTensorField`` (size N)
     - ``(N, 6)``
     - same as scalar

The view is writable. Mutating it from either side is visible on the
other:

.. code-block:: python

   from pybFoam import scalarField
   import numpy as np

   a = scalarField([0.0] * 10)
   b = scalarField([1.0] * 10)
   np_a = np.asarray(a)

   a += b + 5.0           # mutate via the C++ binding
   assert np_a[0] == 6.0  # visible through the NumPy view

   np_a[0:2] = 42.0       # mutate via NumPy
   assert a[0] == 42.0    # visible through the C++ binding

The same contract holds for the ``volScalarField.internalField()`` and
boundary-patch buffers, and for the geometry accessors of sampling
objects (``plane.Cf()``, ``plane.points()``, ``line.distance()``).
