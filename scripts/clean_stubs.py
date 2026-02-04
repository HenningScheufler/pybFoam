#!/usr/bin/env python3
"""
Clean pybFoam_core.pyi by replacing C++ Foam:: types with Python types.
"""

import re
from pathlib import Path

# Type replacements: C++ pattern -> Python type
# IMPORTANT: Order matters! Most complex/specific patterns FIRST, simple patterns LAST
# This prevents partial replacements from breaking complex pattern matching
TYPE_REPLACEMENTS = {
    # ===================================================================
    # MOST COMPLEX PATTERNS FIRST - tmp wrapped geometric fields
    # ===================================================================
    # tmp wrapped geometric fields - volume
    r"Foam::tmp<Foam::GeometricField<double,\s*Foam::fvPatchField,\s*Foam::volMesh>>": "tmp_volScalarField",
    r"Foam::tmp<Foam::GeometricField<Foam::Vector<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>>": "tmp_volVectorField",
    r"Foam::tmp<Foam::GeometricField<Foam::Tensor<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>>": "tmp_volTensorField",
    r"Foam::tmp<Foam::GeometricField<Foam::SymmTensor<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>>": "tmp_volSymmTensorField",
    # tmp wrapped geometric fields - surface
    r"Foam::tmp<Foam::GeometricField<double,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>>": "tmp_surfaceScalarField",
    r"Foam::tmp<Foam::GeometricField<Foam::Vector<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>>": "tmp_surfaceVectorField",
    r"Foam::tmp<Foam::GeometricField<Foam::Tensor<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>>": "tmp_surfaceTensorField",
    r"Foam::tmp<Foam::GeometricField<Foam::SymmTensor<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>>": "tmp_surfaceSymmTensorField",
    # tmp wrapped fields with full type names
    r"Foam::tmp<Foam::Field<double>>": "tmp_scalarField",
    r"Foam::tmp<Foam::Field<Foam::Vector<double>>>": "tmp_vectorField",
    r"Foam::tmp<Foam::Field<Foam::Tensor<double>>>": "tmp_tensorField",
    r"Foam::tmp<Foam::Field<Foam::SymmTensor<double>>>": "tmp_symmTensorField",
    # ===================================================================
    # GEOMETRIC FIELDS - volume and surface
    # ===================================================================
    # Geometric fields - volume
    r"Foam::GeometricField<double,\s*Foam::fvPatchField,\s*Foam::volMesh>": "volScalarField",
    r"Foam::GeometricField<Foam::Vector<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>": "volVectorField",
    r"Foam::GeometricField<Foam::Tensor<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>": "volTensorField",
    r"Foam::GeometricField<Foam::SymmTensor<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>": "volSymmTensorField",
    # Geometric fields - surface
    r"Foam::GeometricField<double,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>": "surfaceScalarField",
    r"Foam::GeometricField<Foam::Vector<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>": "surfaceVectorField",
    r"Foam::GeometricField<Foam::Tensor<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>": "surfaceTensorField",
    r"Foam::GeometricField<Foam::SymmTensor<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>": "surfaceSymmTensorField",
    # ===================================================================
    # FIELD TYPES with full type names
    # ===================================================================
    r"Foam::Field<double>": "scalarField",
    r"Foam::Field<Foam::Vector<double>>": "vectorField",
    r"Foam::Field<Foam::Tensor<double>>": "tensorField",
    r"Foam::Field<Foam::SymmTensor<double>>": "symmTensorField",
    # ===================================================================
    # SOLVER PERFORMANCE with full type names
    # ===================================================================
    r"Foam::SolverPerformance<double>": "SolverScalarPerformance",
    r"Foam::SolverPerformance<Foam::Vector<double>\s*>": "SolverVectorPerformance",
    r"Foam::SolverPerformance<Foam::Tensor<double>\s*>": "SolverTensorPerformance",
    r"Foam::SolverPerformance<Foam::SymmTensor<double>\s*>": "SolverSymmTensorPerformance",
    # ===================================================================
    # DIMENSIONED TYPES with full type names
    # ===================================================================
    r"Foam::dimensioned<double>": "dimensionedScalar",
    r"Foam::dimensioned<Foam::Vector<double>\s*>": "dimensionedVector",
    r"Foam::dimensioned<Foam::Tensor<double>\s*>": "dimensionedTensor",
    r"Foam::dimensioned<Foam::SymmTensor<double>\s*>": "dimensionedSymmTensor",
    # ===================================================================
    # FV MATRIX TYPES with full type names
    # ===================================================================
    r"Foam::fvMatrix<double>": "fvScalarMatrix",
    r"Foam::fvMatrix<Foam::Vector<double>\s*>": "fvVectorMatrix",
    r"Foam::fvMatrix<Foam::Tensor<double>\s*>": "fvTensorMatrix",
    r"Foam::fvMatrix<Foam::SymmTensor<double>\s*>": "fvSymmTensorMatrix",
    # ===================================================================
    # LISTS
    # ===================================================================
    r"Foam::List<Foam::word>": "wordList",
    r"Foam::List<Foam::instant>": "instantList",
    r"Foam::List<int>": "labelList",
    r"Foam::List<bool>": "boolList",
    r"Foam::List<Foam::face>": "faceList",
    r"Foam::polyPatch": "polyPatch",
    r"Foam::fvPatch": "fvPatch",
    r"Foam::polyMesh": "polyMesh",
    r"Foam::polyBoundaryMesh": "polyBoundaryMesh",
    # ===================================================================
    # SIMPLE TYPES - Must come LAST!
    # ===================================================================
    r"<readOption.NO_READ: 0>": "NO_READ",
    r"<writeOption.NO_WRITE: 0>": "NO_WRITE",
    # dictionary
    r"Foam::dictionary": "dictionary",
    # Primitives - these come last to avoid breaking complex patterns
    r"Foam::Vector<double>": "vector",
    r"Foam::Tensor<double>": "tensor",
    r"Foam::SymmTensor<double>": "symmTensor",
    # Simple object types
    r"Foam::word": "Word",
    r"Foam::Time": "Time",
    r"Foam::fvMesh": "fvMesh",
    r"Foam::instant": "instant",
    # buffer error - match with any indentation
    r"(\s*)def __buffer__\(self, flags\):": r"\1def __buffer__(self, flags: int) -> memoryview:",
    r"(\s*)def __release_buffer__\(self, buffer\):": r"\1def __release_buffer__(self, buffer: memoryview) -> None:",
    # Fix untyped dict returns
    r"\) -> dict:": ") -> dict[str, typing.Any]:",
}


def main():
    # Look for stubs directory relative to current working directory
    cwd = Path.cwd()
    stubs_dir = cwd / "stubs"

    # If not found in cwd, try relative to script location
    if not stubs_dir.exists():
        script_dir = Path(__file__).parent.parent
        stubs_dir = script_dir / "stubs"

    if not stubs_dir.exists():
        print(
            f"Error: stubs directory not found (tried {cwd / 'stubs'} and {script_dir / 'stubs'})"
        )
        return

    # Process all .pyi files in stubs directory recursively
    stub_files = list(stubs_dir.glob("**/*.pyi"))

    if not stub_files:
        print(f"No stub files found in {stubs_dir}")
        return

    print(f"Cleaning {len(stub_files)} stub file(s) in {stubs_dir}...")

    for stub_file in stub_files:
        # Show relative path for clarity
        relative_path = stub_file.relative_to(stubs_dir)
        print(f"  Processing {relative_path}...")
        content = stub_file.read_text()
        original = content

        # Normalize whitespace in C++ templates first
        # Fix broken scope operators: "Foam: :Type" -> "Foam::Type"
        content = re.sub(r":\s+:", "::", content)
        # Remove spaces before closing >
        content = re.sub(r"\s+>", ">", content)

        # Apply all replacements
        for pattern, replacement in TYPE_REPLACEMENTS.items():
            content = re.sub(pattern, replacement, content)

        if content != original:
            stub_file.write_text(content)
            print(f"    ✓ Cleaned {relative_path}")
        else:
            print(f"    - No changes needed for {relative_path}")

    print("\nStub cleaning complete!")


if __name__ == "__main__":
    main()
