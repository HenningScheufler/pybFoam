OpenFOAM interop details
========================

pybFoam is not a pure wrapper around a hypothetical "OpenFOAM ABI" — no
such ABI exists. A pybFoam wheel is compiled against **one specific
OpenFOAM build**, inheriting its integer size, floating-point precision,
and API version as compile-time constants. This page explains what that
implies in practice.

The three compile-time knobs
----------------------------

When ``FindOpenFOAM.cmake`` configures the build, it reads three
environment variables and bakes them into every compilation unit:

.. list-table::
   :header-rows: 1
   :widths: 25 35 40

   * - Variable
     - Typical value
     - Effect
   * - ``WM_LABEL_SIZE``
     - ``32`` or ``64``
     - Integer size for ``Foam::label``. Added as ``-DWM_LABEL_SIZE=<n>``.
   * - ``WM_PRECISION_OPTION``
     - ``SP``, ``DP``, ``SPDP``
     - Floating-point precision for ``Foam::scalar``. Added as
       ``-DWM_<option>``.
   * - ``FOAM_API``
     - e.g. ``2506``
     - OpenFOAM API integer. Added as ``-DOPENFOAM=<api>`` so version-gated
       C++ code paths work correctly.

All three must match the OpenFOAM installation the wheel will import
against at runtime. Mixing them is **undefined behaviour** — typical
symptoms are silent numerical corruption (wrong label size), hard crashes
(wrong scalar size), or failed symbol lookups (wrong API version).

Why you should rebuild pybFoam when you switch OpenFOAM versions
----------------------------------------------------------------

There is no way for a pybFoam wheel to detect at import time that the
OpenFOAM install in ``$FOAM_LIBBIN`` disagrees with what it was built
against. The binaries will load, function calls will dispatch, and you
will only notice when a computation produces nonsense.

In practice:

* **Sourcing a different ``etc/bashrc``** between install and use is the
  single most common foot-gun. Always re-install (``pip install -e
  .[all]``) after switching.
* **Changing just ``WM_LABEL_SIZE``** (32 → 64) requires a rebuild even
  with the same API version.
* **Changing ``WM_PRECISION_OPTION``** (``DP`` → ``SP``) changes
  ``sizeof(scalar)`` — every field buffer, every dictionary accessor,
  every NumPy view has to be rebuilt.

The CI matrix
-------------

``.github/workflows/ci.yaml`` exercises OpenFOAM **2312, 2406, 2412, 2506,
2512** in separate jobs, each with a fresh build. That is the set of API
versions explicitly supported. Building against versions outside this
range usually works but is not guaranteed — the ``FOAM_API`` macro is how
source code guards compatibility, so occasional breakage is expected at
the edges.

Label size in Python-land
-------------------------

The Python side does not expose ``WM_LABEL_SIZE`` directly. From Python,
sizes come back as Python ``int`` (arbitrary precision) and NumPy arrays
of ``np.int32`` or ``np.int64`` depending on the underlying OpenFOAM
build. If you write code that writes back into an integer field or list,
prefer reading the dtype off the NumPy view rather than hardcoding a type:

.. code-block:: python

   import numpy as np
   from pybFoam import labelList

   cells = np.asarray(some_labelList)
   print(cells.dtype)          # int32 or int64 depending on WM_LABEL_SIZE

Parallel runs
-------------

Each MPI rank loads its own copy of the extension modules and the
OpenFOAM libraries. The ABI constraints above apply per rank — but since
every rank is launched from the same Python environment with the same
sourced OpenFOAM, this is usually transparent. Consistency issues only
arise if individual ranks source different OpenFOAM installs, which is
pathological.
