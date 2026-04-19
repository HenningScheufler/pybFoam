# pybFoam

[![Documentation](https://img.shields.io/badge/docs-latest-blue)](https://henningscheufler.github.io/pybFoam/)
[![Deploy Docs](https://github.com/HenningScheufler/pybFoam/actions/workflows/pages.yaml/badge.svg)](https://github.com/HenningScheufler/pybFoam/actions/workflows/pages.yaml)

Python bindings for OpenFOAM - enabling direct manipulation of OpenFOAM cases, fields, and meshes from Python.

**[Documenation](https://henningscheufler.github.io/pybFoam/)** — tutorials, how-to guides (generated from runnable examples), API reference, and design notes.

Build the docs locally:

```bash
uv sync --extra docs
cd docs && uv run make html
# open docs/_build/html/index.html
```

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

- **OpenFOAM**: v2312 or higher (sourced and installed)
- **Python**: 3.9 or higher
- **CMake**: 3.18 or higher
- **C++ Compiler**: C++17 compatible (TBD)
- **Build tools**: nanobind, scikit-build-core
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
pip install -e .[all]
```

#### Generating Type Stubs (Development only)

Type stubs (.pyi files) are generated post-installation using a separate script:

```bash
# Install the package first
uv pip install -e .[all]

# Generate and verify stubs
./scripts/generate_stubs.sh
```

This script:
1. Generates stubs using pybind11-stubgen
2. Cleans and formats the stubs
3. Copies them to the source directory
4. Verifies them with mypy

---

## Quick Start

### Basic Usage

```python
import pybFoam as pf

# Create OpenFOAM time and mesh
time = pf.Time(".", ".")
mesh = pf.fvMesh(time)

# Access fields
p_rgh = pf.volScalarField.read_field(mesh, "p_rgh")
U = pf.volVectorField.read_field(mesh, "U")

# Compute gradients using finite volume calculus
grad_p = pf.fvc.grad(p_rgh)
div_U = pf.fvc.div(U)

# Convert to NumPy arrays for analysis
import numpy as np
p_array = np.asarray(p_rgh["internalField"])
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

### Read OpenFOAM dictionaries

```python
from pybFoam import Word, dictionary

d = dictionary.read("system/controlDict")

# Typed accessors — a typo in the key raises at the call site
application = d.get[Word]("application")
end_time    = d.get[float]("endTime")
max_co      = d.getOrDefault[float]("maxCo", 0.5)

# Nested subdictionaries and key enumeration
piso = d.subDict("PISO")
for name in d.toc():
    print(name)
```

---

## Examples

Runnable, self-contained scripts live under `examples/`. They are
executed at documentation build time and rendered as the tutorials and
how-to guides on the docs site.

- **Tutorials** (`examples/tutorials/`) — end-to-end walkthroughs that
  build on each other:
  - `example_01_getting_started.py` — open a case, read a field, NumPy view
  - `example_02_field_analysis.py` — drive a time loop and summarise
    per-step statistics
  - `example_03_sampling_workflow.py` — sample a plane, export to CSV
- **How-to guides** (`examples/how-to/`) — task-focused recipes:
  - `example_read_dictionaries.py` — controlDict / typed entries
  - `example_blockmesh.py` — `generate_blockmesh` + `checkMesh`
  - `example_fvc_fvm_operators.py` — gradient / divergence / Laplacian
    and implicit matrix terms
  - `example_sample_plane.py`, `example_sample_line.py`
- **Case data** — `examples/case/` and `examples/cavity/` are
  read-only baseline cases consumed by the scripts above.
- **Solver example** — `examples/cavity/icoFoam.py` (full PISO loop
  driven from Python).

More tests with additional API usage live under `tests/pybind/`.

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

### Development Setup

1. Clone the repository
2. Install development dependencies:
   ```bash
   pip install -e ".[dev]"
   # or build with stubs
   pip install -e .[all] -C cmake.define.ENABLE_PYBFOAM_STUBS=ON -v
   ```
3. Run tests before committing:
   ```bash
   pytest tests/
   ```

---

## License

See [LICENSE](LICENSE) file for details.

---
