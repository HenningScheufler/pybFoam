# Installation Guide for pybFoam

## Prerequisites

1. OpenFOAM (v2406 or compatible) must be installed and sourced
2. Python 3.8 or higher
3. CMake 3.18 or higher
4. A C++ compiler with C++17 support

## Installation Methods

### Method 1: Pip Installation (Recommended)

1. **Source OpenFOAM environment:**
   ```bash
   source $FOAM_INSTALL_DIR/etc/bashrc
   ```

2. **Install in development mode:**
   ```bash
   pip install -e .
   ```

3. **Or install from source:**
   ```bash
   pip install .
   ```

### Method 2: Manual CMake Build

1. **Create build directory:**
   ```bash
   mkdir build && cd build
   ```

2. **Configure with CMake:**
   ```bash
   cmake ..
   ```

3. **Build:**
   ```bash
   make -j$(nproc)
   ```

4. **Install:**
   ```bash
   make install
   ```

## Development Installation

For development, install with additional dependencies:

```bash
# Install with all development dependencies
pip install -e ".[dev,docs,viz]"

# Or install specific optional dependency groups
pip install -e ".[dev]"        # Development tools (pytest, black, etc.)
pip install -e ".[docs]"       # Documentation tools (sphinx, etc.)
pip install -e ".[viz]"        # Visualization tools (matplotlib)
pip install -e ".[test]"       # Testing tools (oftest)
pip install -e ".[all]"        # All optional dependencies

# Legacy method (still works)
pip install -r requirements-dev.txt
```

### Available Optional Dependencies

- **dev**: Development tools (pytest, black, flake8, mypy, cmake, ninja)
- **docs**: Documentation tools (sphinx, sphinx-rtd-theme, myst-parser)
- **viz**: Visualization tools (matplotlib)
- **test**: Testing frameworks (oftest)
- **all**: All optional dependencies combined

## Testing Installation

Run the test script to verify the installation:

```bash
./test_pip_install.sh
```

## Usage

```python
import pybFoam

# Use the library
print(f"pybFoam version: {pybFoam.__version__}")
```

## Troubleshooting

### OpenFOAM Environment Not Sourced
If you get an error about OpenFOAM environment variables, make sure to source OpenFOAM:

```bash
source $FOAM_INSTALL_DIR/etc/bashrc
```

### CMake Build Issues
If CMake cannot find OpenFOAM, ensure that `FOAM_SRC` and `FOAM_LIBBIN` environment variables are set.

### Compilation Errors
Make sure you have a compatible C++ compiler and that OpenFOAM is properly compiled and installed.
