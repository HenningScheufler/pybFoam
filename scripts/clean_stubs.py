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
    r'Foam::tmp<Foam::GeometricField<double,\s*Foam::fvPatchField,\s*Foam::volMesh>\s*>': 'pybFoam.pybFoam_core.tmp_volScalarField',
    r'Foam::tmp<Foam::GeometricField<Foam::Vector<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>\s*>': 'pybFoam.pybFoam_core.tmp_volVectorField',
    r'Foam::tmp<Foam::GeometricField<Foam::Tensor<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>\s*>': 'pybFoam.pybFoam_core.tmp_volTensorField',
    r'Foam::tmp<Foam::GeometricField<Foam::SymmTensor<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>\s*>': 'pybFoam.pybFoam_core.tmp_volSymmTensorField',
    
    # tmp wrapped geometric fields - surface
    r'Foam::tmp<Foam::GeometricField<double,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>\s*>': 'pybFoam.pybFoam_core.tmp_surfaceScalarField',
    r'Foam::tmp<Foam::GeometricField<Foam::Vector<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>\s*>': 'pybFoam.pybFoam_core.tmp_surfaceVectorField',
    r'Foam::tmp<Foam::GeometricField<Foam::Tensor<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>\s*>': 'pybFoam.pybFoam_core.tmp_surfaceTensorField',
    r'Foam::tmp<Foam::GeometricField<Foam::SymmTensor<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>\s*>': 'pybFoam.pybFoam_core.tmp_surfaceSymmTensorField',
    
    # tmp wrapped fields with full type names
    r'Foam::tmp<Foam::Field<double>\s*>': 'pybFoam.pybFoam_core.tmp_scalarField',
    r'Foam::tmp<Foam::Field<Foam::Vector<double>>\s*>': 'pybFoam.pybFoam_core.tmp_vectorField',
    r'Foam::tmp<Foam::Field<Foam::Tensor<double>>\s*>': 'pybFoam.pybFoam_core.tmp_tensorField',
    r'Foam::tmp<Foam::Field<Foam::SymmTensor<double>>\s*>': 'pybFoam.pybFoam_core.tmp_symmTensorField',
    
    # ===================================================================
    # GEOMETRIC FIELDS - volume and surface
    # ===================================================================
    
    # Geometric fields - volume
    r'Foam::GeometricField<double,\s*Foam::fvPatchField,\s*Foam::volMesh>': 'pybFoam.pybFoam_core.volScalarField',
    r'Foam::GeometricField<Foam::Vector<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>': 'pybFoam.pybFoam_core.volVectorField',
    r'Foam::GeometricField<Foam::Tensor<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>': 'pybFoam.pybFoam_core.volTensorField',
    r'Foam::GeometricField<Foam::SymmTensor<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>': 'pybFoam.pybFoam_core.volSymmTensorField',
    
    # Geometric fields - surface
    r'Foam::GeometricField<double,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>': 'pybFoam.pybFoam_core.surfaceScalarField',
    r'Foam::GeometricField<Foam::Vector<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>': 'pybFoam.pybFoam_core.surfaceVectorField',
    r'Foam::GeometricField<Foam::Tensor<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>': 'pybFoam.pybFoam_core.surfaceTensorField',
    r'Foam::GeometricField<Foam::SymmTensor<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>': 'pybFoam.pybFoam_core.surfaceSymmTensorField',
    
    # ===================================================================
    # FIELD TYPES with full type names
    # ===================================================================
    
    r'Foam::Field<double>': 'pybFoam.pybFoam_core.scalarField',
    r'Foam::Field<Foam::Vector<double>>': 'pybFoam.pybFoam_core.vectorField',
    r'Foam::Field<Foam::Tensor<double>>': 'pybFoam.pybFoam_core.tensorField',
    r'Foam::Field<Foam::SymmTensor<double>>': 'pybFoam.pybFoam_core.symmTensorField',
    
    # ===================================================================
    # SOLVER PERFORMANCE with full type names
    # ===================================================================
    
    r'Foam::SolverPerformance<double>': 'pybFoam.pybFoam_core.SolverScalarPerformance',
    r'Foam::SolverPerformance<Foam::Vector<double>\s*>': 'pybFoam.pybFoam_core.SolverVectorPerformance',
    r'Foam::SolverPerformance<Foam::Tensor<double>\s*>': 'pybFoam.pybFoam_core.SolverTensorPerformance',
    r'Foam::SolverPerformance<Foam::SymmTensor<double>\s*>': 'pybFoam.pybFoam_core.SolverSymmTensorPerformance',
    
    # ===================================================================
    # DIMENSIONED TYPES with full type names
    # ===================================================================
    
    r'Foam::dimensioned<double>': 'pybFoam.pybFoam_core.dimensionedScalar',
    r'Foam::dimensioned<Foam::Vector<double>\s*>': 'pybFoam.pybFoam_core.dimensionedVector',
    r'Foam::dimensioned<Foam::Tensor<double>\s*>': 'pybFoam.pybFoam_core.dimensionedTensor',
    r'Foam::dimensioned<Foam::SymmTensor<double>\s*>': 'pybFoam.pybFoam_core.dimensionedSymmTensor',
    
    # ===================================================================
    # FV MATRIX TYPES with full type names
    # ===================================================================
    
    r'Foam::fvMatrix<double>': 'pybFoam.pybFoam_core.fvScalarMatrix',
    r'Foam::fvMatrix<Foam::Vector<double>\s*>': 'pybFoam.pybFoam_core.fvVectorMatrix',
    r'Foam::fvMatrix<Foam::Tensor<double>\s*>': 'pybFoam.pybFoam_core.fvTensorMatrix',
    r'Foam::fvMatrix<Foam::SymmTensor<double>\s*>': 'pybFoam.pybFoam_core.fvSymmTensorMatrix',
    
    # ===================================================================
    # LISTS
    # ===================================================================
    
    r'Foam::List<Foam::word>': 'pybFoam.pybFoam_core.wordList',
    r'Foam::List<Foam::instant>': 'pybFoam.pybFoam_core.instantList',
    r'Foam::List<int>': 'pybFoam.pybFoam_core.labelList',
    r'Foam::List<bool>': 'pybFoam.pybFoam_core.boolList',
    r'Foam::List<Foam::face>': 'pybFoam.pybFoam_core.faceList',
    
    # ===================================================================
    # SIMPLE TYPES - Must come LAST!
    # ===================================================================
    
    # dictionary
    r'Foam::dictionary': 'pybFoam.pybFoam_core.dictionary',
    
    # Primitives - these come last to avoid breaking complex patterns
    r'Foam::Vector<double>': 'pybFoam.pybFoam_core.vector',
    r'Foam::Tensor<double>': 'pybFoam.pybFoam_core.tensor',
    r'Foam::SymmTensor<double>': 'pybFoam.pybFoam_core.symmTensor',
    
    # Simple object types
    r'Foam::word': 'pybFoam.pybFoam_core.Word',
    r'Foam::Time': 'pybFoam.pybFoam_core.Time',
    r'Foam::fvMesh': 'pybFoam.pybFoam_core.fvMesh',
    r'Foam::instant': 'pybFoam.pybFoam_core.instant',
}


def main():
    # Look for stubs directory relative to current working directory
    cwd = Path.cwd()
    stubs_dir = cwd / 'stubs'
    
    # If not found in cwd, try relative to script location
    if not stubs_dir.exists():
        script_dir = Path(__file__).parent.parent
        stubs_dir = script_dir / 'stubs'
    
    if not stubs_dir.exists():
        print(f"Error: stubs directory not found (tried {cwd / 'stubs'} and {script_dir / 'stubs'})")
        return
    
    # Process all .pyi files in stubs directory recursively
    stub_files = list(stubs_dir.glob('**/*.pyi'))
    
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
        
        # Apply all replacements
        for pattern, replacement in TYPE_REPLACEMENTS.items():
            content = re.sub(pattern, replacement, content)
        
        if content != original:
            stub_file.write_text(content)
            print(f"    ✓ Cleaned {relative_path}")
        else:
            print(f"    - No changes needed for {relative_path}")
    
    print(f"\nStub cleaning complete!")


if __name__ == '__main__':
    main()
