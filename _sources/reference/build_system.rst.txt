Build system
============

pybFoam is built with `scikit-build-core`_ driving CMake and `nanobind`_. This
page documents the environment variables, CMake options, and CMake targets you
may need to know about when building or packaging pybFoam.

.. _scikit-build-core: https://scikit-build-core.readthedocs.io/
.. _nanobind: https://nanobind.readthedocs.io/

Required environment
--------------------

``cmake/FindOpenFOAM.cmake`` is a hard gate: configuration fails immediately
if any of these environment variables are unset. Sourcing
``/path/to/OpenFOAM/etc/bashrc`` defines all of them.

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - Variable
     - Purpose
   * - ``FOAM_SRC``
     - Root of OpenFOAM source headers; used to find ``fvCFD.H`` and every
       ``lnInclude/`` directory.
   * - ``FOAM_LIBBIN``
     - Directory containing the built OpenFOAM shared libraries
       (``libOpenFOAM.so``, ``libfiniteVolume.so``, …).
   * - ``WM_LABEL_SIZE``
     - Integer label size (32 or 64). Compiled in as
       ``-DWM_LABEL_SIZE=<n>`` — must match the OpenFOAM build.
   * - ``WM_PRECISION_OPTION``
     - Floating-point precision (``SP``, ``DP``, ``SPDP``). Compiled in as
       ``-DWM_<option>``.
   * - ``FOAM_API``
     - OpenFOAM API integer (e.g. ``2506``). Compiled in as
       ``-DOPENFOAM=<api>`` so version-gated code paths can ``#if`` on it.

CMake options
-------------

.. list-table::
   :header-rows: 1
   :widths: 30 20 50

   * - Option
     - Default
     - Effect
   * - ``PYBFOAM_BUILD_EMBED``
     - ``ON``
     - Build ``libpybFoamEmbed.so``, the C++ library that OpenFOAM solvers
       link against to embed Python. Turn off when OpenFOAM is unavailable
       or not needed.
   * - ``ENABLE_PYBFOAM_STUBS``
     - ``OFF``
     - Generate ``.pyi`` type stubs during the build. Enable with
       ``-C cmake.define.ENABLE_PYBFOAM_STUBS=ON`` on the ``pip install``
       command. See also :doc:`../how-to/read_write_dictionaries` for the
       standalone ``scripts/generate_stubs.sh`` workflow.
   * - ``CMAKE_CXX_STANDARD``
     - ``17``
     - Set via ``[tool.scikit-build.cmake.define]`` in ``pyproject.toml``.
       Do not lower — OpenFOAM v2312+ requires C++17.

scikit-build-core configuration
-------------------------------

From ``pyproject.toml``:

.. code-block:: toml

   [tool.scikit-build]
   cmake.version = ">=3.18"
   cmake.build-type = "Release"
   build.verbose = true
   build-dir = "_skbuild"
   editable.mode = "redirect"
   editable.verbose = true
   editable.rebuild = false

Note that ``editable.rebuild = false`` — edits to the **C++** sources require
a manual ``pip install -e .[all]`` to take effect. Pure-Python edits are
picked up live.

CMake targets exposed by ``FindOpenFOAM``
-----------------------------------------

Each target is an ``INTERFACE IMPORTED`` library that pulls in the relevant
OpenFOAM headers and shared libraries.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Target
     - Wraps
   * - ``OpenFOAM::core``
     - ``libOpenFOAM`` + OSspecific/POSIX
   * - ``OpenFOAM::fileFormats``
     - ``libfileFormats``, ``libsurfMesh``
   * - ``OpenFOAM::meshTools``
     - ``libmeshTools``, ``libsurfMesh``, ``libdynamicMesh`` headers
   * - ``OpenFOAM::finiteVolume``
     - ``libfiniteVolume``, ``libdynamicMesh``, ``libdynamicFvMesh``
   * - ``OpenFOAM::thermo``
     - ``libfluidThermophysicalModels``, ``libsolidThermo``, ``libspecie``
   * - ``OpenFOAM::turbulence``
     - ``libturbulenceModels`` (+ incompressible/compressible variants)
   * - ``OpenFOAM::transport``
     - ``libincompressibleTransportModels``, ``libcompressibleTransportModels``
   * - ``OpenFOAM::lagrangian``
     - ``liblagrangian``
   * - ``OpenFOAM::sampling``
     - ``libsampling`` (pulls in ``finiteVolume`` + ``meshTools``)
   * - ``OpenFOAM::api``
     - Umbrella target linking all of the above

Install layout
--------------

A non-editable install lays out ``site-packages/pybFoam/`` as:

.. code-block:: text

   pybFoam/
   ├── *.so                    # nanobind extension modules
   ├── libnanobind.so          # shared nanobind runtime (RPATH=$ORIGIN)
   ├── embed/
   │   ├── lib/libpybFoamEmbed.so
   │   ├── include/pybFoamEmbed/
   │   └── cmake/pybFoamEmbed/  # pybFoamEmbedConfig.cmake + Targets
   └── ...

The ``verify-embed-install`` CI job checks this layout — see
``.github/workflows/ci.yaml``.
