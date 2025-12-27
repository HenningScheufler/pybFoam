# pybFoam

Python bindings for OpenFOAM - enabling direct manipulation of OpenFOAM cases, fields, and meshes from Python.

Currently in the pre-alpha release state.

---

## Features

- **Direct Python access to OpenFOAM data structures**: Time, fvMesh, fields
- **Finite volume operators**: fvc (calculus), fvm (matrix operations)
- **Turbulence and thermodynamic models**: Access to OpenFOAM turbulence and thermo libraries
- **Sampling and post-processing**: Surface sampling, line sampling, interpolation
- **Pydantic configuration models**: Type-safe dictionary construction for sampling surfaces
- **NumPy integration**: Zero-copy access to OpenFOAM field data via buffer protocol

---

## Requirements

- **OpenFOAM**: v2012 or higher (sourced and installed)
- **Python**: 3.8 or higher
- **CMake**: 3.18 or higher
- **C++ Compiler**: C++17 compatible (GCC 7+, Clang 5+)
- **Build tools**: pybind11, scikit-build-core
- **Python packages**: numpy, pydantic

---

## Installation

### Prerequisites

1. Source your OpenFOAM environment:
   ```bash
   source /path/to/OpenFOAM/etc/bashrc
   ```

2. (Recommended) Create a virtual environment:
   ```bash
   python -m venv .venv
   source .venv/bin/activate
   ```

### Install from source

```bash
pip install .
```

For development:
```bash
pip install -e . --no-build-isolation
```

---

## Quick Start

### Basic Usage

```python
import pybFoam as pf

# Create OpenFOAM time and mesh
time = pf.createTime()
mesh = pf.fvMesh(time)

# Access fields
p = pf.volScalarField.read_field(mesh, "p")
U = pf.volVectorField.read_field(mesh, "U")

# Compute gradients using finite volume calculus
grad_p = pf.fvc.grad(p)
div_U = pf.fvc.div(U)

# Convert to NumPy arrays for analysis
import numpy as np
p_array = np.asarray(p["internalField"])
print(f"Pressure range: {p_array.min():.3f} to {p_array.max():.3f}")
```

### Sampling Surfaces

```python
from pybFoam.sampling import SampledPlaneConfig, sampledSurface, interpolationScalar
from pybFoam import Word

# Create a sampling plane using Pydantic config
plane_config = SampledPlaneConfig(
    point=[0.5, 0.5, 0.0],
    normal=[1.0, 0.0, 0.0]
)

# Create the surface
plane = sampledSurface.New(Word("myPlane"), mesh, plane_config.to_foam_dict())
plane.update()

# Interpolate field onto surface
interp = interpolationScalar.New(Word("cellPoint"), p)
sampled_values = interp.sampleOnFaces(plane)
```

### Dictionary I/O with Pydantic

```python
from pybFoam.io import IOModelBase
from pydantic import Field

class TransportProperties(IOModelBase):
    nu: float = Field(..., description="Kinematic viscosity")
    
    class Config:
        foam_file_name = "transportProperties"

# Read from OpenFOAM dictionary
props = TransportProperties.from_file("constant/transportProperties")
print(f"Viscosity: {props.nu}")

# Modify and write back
props.nu = 1e-5
props.to_file("constant/transportProperties")
```

---

## Examples

See the `tests/` directory for more examples:
- **Basic field operations**: `tests/pybind/test_primitives.py`
- **Finite volume operators**: `tests/pybind/test_fvc.py`, `tests/pybind/test_fvm.py`
- **Surface sampling**: `tests/pybind/test_surface_sampling.py`
- **Line sampling**: `tests/pybind/test_set_sampling.py`
- **Solver example**: `tests/cavity/icoFoam.py`

---

## Testing

Run the test suite:

```bash
pytest tests/
```

Run specific test categories:
```bash
pytest tests/pybind/          # C++ binding tests
pytest tests/test_sampling_models.py  # Pydantic config tests
```

---

## Documentation

Full documentation is available at: [https://henningscheufler.github.io/pybFoam/](https://henningscheufler.github.io/pybFoam/index.html)

---

### Development Setup

1. Clone the repository
2. Install development dependencies:
   ```bash
   pip install -e ".[dev]"
   ```
3. Run tests before committing:
   ```bash
   pytest tests/
   ```

---

## License

See [LICENSE](LICENSE) file for details.

---


