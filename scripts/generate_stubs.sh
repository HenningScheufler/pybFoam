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
cp -r "$STUBS_DIR/pybFoam/"* "$SRC_DIR/"
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
