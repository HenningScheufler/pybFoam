Supported versions
==================

pybFoam is tested in CI against the following combinations. Versions outside
this matrix may work but are not exercised on every push.

OpenFOAM × Python
-----------------

.. list-table::
   :header-rows: 1
   :widths: 20 20 60

   * - OpenFOAM
     - Python
     - Notes
   * - 2312
     - 3.11
     - Minimum supported OpenFOAM API.
   * - 2406
     - 3.11
     -
   * - 2412
     - 3.11
     -
   * - 2506
     - 3.11
     - Also used for the pre-commit and ``pybFoamEmbed`` install-layout jobs.
   * - 2512
     - 3.11
     - Latest tested.

Source: ``.github/workflows/ci.yaml`` (``matrix.openfoam-version``).

Python
------

The ``pyproject.toml`` declares ``requires-python = ">=3.9"`` and ships
classifiers for **3.9**, **3.10**, **3.11**, and **3.12**. CI currently
exercises only 3.11 — other Python versions in that range should work but
are untested.

Build-time requirements
-----------------------

* **CMake** 3.18 or higher
* **C++17** compiler (GCC ≥ 9 or Clang ≥ 9)
* **nanobind** ≥ 1.8.0 (fetched via scikit-build-core)
* **scikit-build-core** ≥ 0.4.3

Runtime dependencies
--------------------

From ``pyproject.toml``:

.. code-block:: toml

   dependencies = [
       "numpy>=1.20",
       "nanobind>=1.8.0",
       "pydantic",
       "pyyaml",
   ]

Optional dependency groups: ``dev``, ``docs``, ``benchmark``, ``all``.
