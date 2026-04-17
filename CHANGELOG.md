# Changelog

## [0.4.3]

* fix segfault on Python 3.10/3.11 when loading the embedded interpreter:
  - removed a `nb_module_exec(NB_DOMAIN_STR, nullptr)` call from `pyInterp`
    that dereferenced a null module pointer.
  - unified nanobind linkage: all extension modules (`pybFoam_core`, `fvc`,
    `fvm`, `thermo`, `turbulence`, `sampling`, `meshing`, `runTimeTables`)
    now use `NB_SHARED`

## [0.4.2]

* fix installation of the embedded interpreter in site package

## [0.4.1]

* added additional free functions
* fix installation of the embedded interpreter

## [0.4.0]

* interpreter added
* boolList accepts numpy arrays
* pstream bindings

## [0.3.3]

* wallDist and nearWallDist


## [0.3.2]

### Fixes

* missing std/tuple
* fix invalid free by returnin std::shared_ptr

## [0.3.1]

### Added

* runTime.output

## [0.3.0]

* port to nanobind


## [0.2.0]

## Added

* blockmesh bindings
* snappy hex mesh bindings
* check mesh bindings


### Added
- Initial implementation of sampling module with pybind11 bindings
- Support for sampledSurface types (plane, patch, cutting plane, iso-surface)
- Interpolation schemes (cell, cellPoint, cellPointFace)
- Line sampling functionality (uniform, cloud, polyLine, circle)
- Integration tests for surface and set sampling

### Changed
- Updated build system to use scikit-build-core
- Improved CMake configuration for OpenFOAM library detection

---

## [-0.1.5]

### Added
- Python bindings for OpenFOAM core functionality
- Field access and manipulation (volScalarField, volVectorField, volTensorField)
- Finite volume operators (fvc, fvm)
- Mesh access (fvMesh, polyMesh)
- Time management
- Dictionary I/O
- Basic turbulence model support
- NumPy buffer protocol integration for zero-copy field access

### Known Issues
- Memory leaks in `selectTimes` function (requires RAII fix)
- Incorrect return value policies in some bindings
- Missing negative index checks in field accessors

---
