# Changelog

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
