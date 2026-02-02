#!/bin/bash
# Generate type stubs for pybFoam after installation
# Usage: ./scripts/generate_stubs.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
STUBS_DIR="$PROJECT_ROOT/stubs"
SRC_DIR="$PROJECT_ROOT/src/pybFoam"

echo "Project Root: $PROJECT_ROOT"
echo "Source Directory: $SRC_DIR"
echo "Stubs Directory: $STUBS_DIR"

echo "==================================="
echo "pybFoam Stub Generator"
echo "==================================="
echo ""

# Check if pybFoam is installed
if ! uv run python -c "import pybFoam" 2>/dev/null; then
    echo "Error: pybFoam is not installed!"
    echo "Please install first: uv pip install -e .[all]"
    exit 1
fi

echo "✓ pybFoam is installed"
echo ""

# Generate stubs
echo "Generating stubs..."
rm -rf "$STUBS_DIR"
uv run pybind11-stubgen -o "$STUBS_DIR" --print-invalid-expressions-as-is pybFoam

if [ ! -d "$STUBS_DIR/pybFoam" ]; then
    echo "Error: Stub generation failed!"
    exit 1
fi

echo "✓ Stubs generated in $STUBS_DIR"
echo ""

# Clean stubs
echo "Cleaning stubs..."
uv run python "$SCRIPT_DIR/clean_stubs.py"
echo "✓ Stubs cleaned"
echo ""

# Copy stubs to source directory
echo "Copying stubs to source directory..."

# Copy top-level .pyi files
cp "$STUBS_DIR/pybFoam/__init__.pyi" "$SRC_DIR/"
cp "$STUBS_DIR/pybFoam/_version.pyi" "$SRC_DIR/"
cp "$STUBS_DIR/pybFoam/pybFoam_core.pyi" "$SRC_DIR/"
cp "$STUBS_DIR/pybFoam/sampling_bindings.pyi" "$SRC_DIR/"

# Copy directory-based module stubs
cp "$STUBS_DIR/pybFoam/fvm.pyi" "$SRC_DIR/fvm/__init__.pyi"
cp "$STUBS_DIR/pybFoam/fvc.pyi" "$SRC_DIR/fvc/__init__.pyi"

cp "$STUBS_DIR/pybFoam/meshing.pyi" "$SRC_DIR/meshing/__init__.pyi"
cp "$STUBS_DIR/pybFoam/thermo.pyi" "$SRC_DIR/thermo/__init__.pyi"
cp "$STUBS_DIR/pybFoam/turbulence.pyi" "$SRC_DIR/turbulence/__init__.pyi"
cp "$STUBS_DIR/pybFoam/runTimeTables.pyi" "$SRC_DIR/runTimeTables/__init__.pyi"

echo "✓ Stubs copied to $SRC_DIR"
echo ""

# Verify with mypy
echo "Verifying stubs with mypy..."
if uv run mypy --version >/dev/null 2>&1; then
    echo ""
    echo "Running mypy on pybFoam..."
    uv run mypy "$SRC_DIR" --show-error-codes || true
    echo ""
else
    echo "Warning: mypy not installed, skipping verification"
    echo "Install with: uv pip install mypy"
fi

echo "==================================="
echo "✓ Stub generation complete!"
echo "==================================="
echo ""
echo "Generated stub files:"
find "$SRC_DIR" -name "*.pyi" -type f | sort
