#!/usr/bin/env python3
"""
Clean pybFoam_core.pyi by replacing C++ Foam:: types with Python types.
"""

import re
from pathlib import Path

# Type replacements: C++ pattern -> Python type
TYPE_REPLACEMENTS = {
    # Geometric fields - volume
    r'Foam::GeometricField<double,\s*Foam::fvPatchField,\s*Foam::volMesh>': 'volScalarField',
    r'Foam::GeometricField<Foam::Vector<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>': 'volVectorField',
    r'Foam::GeometricField<Foam::Tensor<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>': 'volTensorField',
    r'Foam::GeometricField<Foam::SymmTensor<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>': 'volSymmTensorField',
    
    # Geometric fields - surface
    r'Foam::GeometricField<double,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>': 'surfaceScalarField',
    r'Foam::GeometricField<Foam::Vector<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>': 'surfaceVectorField',
    r'Foam::GeometricField<Foam::Tensor<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>': 'surfaceTensorField',
    r'Foam::GeometricField<Foam::SymmTensor<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>': 'surfaceSymmTensorField',
    
    # Fields
    r'Foam::Field<double>': 'scalarField',
    r'Foam::Field<Foam::Vector<double>>': 'vectorField',
    r'Foam::Field<Foam::Tensor<double>>': 'tensorField',
    r'Foam::Field<Foam::SymmTensor<double>>': 'symmTensorField',
    
    # tmp wrapped geometric fields - volume
    r'Foam::tmp<Foam::GeometricField<double,\s*Foam::fvPatchField,\s*Foam::volMesh>\s*>': 'tmp_volScalarField',
    r'Foam::tmp<Foam::GeometricField<Foam::Vector<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>\s*>': 'tmp_volVectorField',
    r'Foam::tmp<Foam::GeometricField<Foam::Tensor<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>\s*>': 'tmp_volTensorField',
    r'Foam::tmp<Foam::GeometricField<Foam::SymmTensor<double>,\s*Foam::fvPatchField,\s*Foam::volMesh>\s*>': 'tmp_volSymmTensorField',
    
    # tmp wrapped geometric fields - surface
    r'Foam::tmp<Foam::GeometricField<double,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>\s*>': 'tmp_surfaceScalarField',
    r'Foam::tmp<Foam::GeometricField<Foam::Vector<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>\s*>': 'tmp_surfaceVectorField',
    r'Foam::tmp<Foam::GeometricField<Foam::Tensor<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>\s*>': 'tmp_surfaceTensorField',
    r'Foam::tmp<Foam::GeometricField<Foam::SymmTensor<double>,\s*Foam::fvsPatchField,\s*Foam::surfaceMesh>\s*>': 'tmp_surfaceSymmTensorField',
    
    # tmp wrapped fields
    r'Foam::tmp<Foam::Field<double>\s*>': 'tmp_scalarField',
    r'Foam::tmp<Foam::Field<Foam::Vector<double>>\s*>': 'tmp_vectorField',
    r'Foam::tmp<Foam::Field<Foam::Tensor<double>>\s*>': 'tmp_tensorField',
    r'Foam::tmp<Foam::Field<Foam::SymmTensor<double>>\s*>': 'tmp_symmTensorField',

    # dictionary
    r'Foam::dictionary': 'dictionary',
    
    # Primitives
    r'Foam::Vector<double>': 'vector',
    r'Foam::Tensor<double>': 'tensor',
    r'Foam::SymmTensor<double>': 'symmTensor',
    
    # SolverPerformance
    r'Foam::SolverPerformance<double>': 'SolverScalarPerformance',
    r'Foam::SolverPerformance<Foam::Vector<double>\s*>': 'SolverVectorPerformance',
    r'Foam::SolverPerformance<Foam::Tensor<double>\s*>': 'SolverTensorPerformance',
    r'Foam::SolverPerformance<Foam::SymmTensor<double>\s*>': 'SolverSymmTensorPerformance',
    
    # Lists
    r'Foam::List<Foam::word>': 'wordList',
    r'Foam::List<Foam::instant>': 'instantList',
    r'Foam::List<int>': 'labelList',
    r'Foam::List<bool>': 'boolList',
    
    # Simple types
    r'Foam::word': 'Word',
    r'Foam::Time': 'Time',
    r'Foam::fvMesh': 'fvMesh',
    r'Foam::instant': 'instant',
    r'Foam::nearWallDist': 'nearWallDist',
    
    # Additional patterns for partially replaced types
    r'Foam::Field<vector\s*>': 'vectorField',
    r'Foam::Field<tensor\s*>': 'tensorField',
    r'Foam::Field<symmTensor\s*>': 'symmTensorField',
    r'Foam::GeometricField<double,\s*Foam:\s*:fvPatchField,\s*Foam:\s*:volMesh>': 'volScalarField',
    r'Foam::GeometricField<vector,\s*Foam:\s*:fvPatchField,\s*Foam:\s*:volMesh>': 'volVectorField',
    r'Foam::GeometricField<tensor,\s*Foam:\s*:fvPatchField,\s*Foam:\s*:volMesh>': 'volTensorField',
    r'Foam::GeometricField<symmTensor,\s*Foam:\s*:fvPatchField,\s*Foam:\s*:volMesh>': 'volSymmTensorField',
    r'Foam::GeometricField<double,\s*Foam:\s*:fvsPatchField,\s*Foam:\s*:surfaceMesh>': 'surfaceScalarField',
    r'Foam::GeometricField<vector,\s*Foam:\s*:fvsPatchField,\s*Foam:\s*:surfaceMesh>': 'surfaceVectorField',
    r'Foam::GeometricField<tensor,\s*Foam:\s*:fvsPatchField,\s*Foam:\s*:surfaceMesh>': 'surfaceTensorField',
    r'Foam::GeometricField<symmTensor,\s*Foam:\s*:fvsPatchField,\s*Foam:\s*:surfaceMesh>': 'surfaceSymmTensorField',
    r'Foam::tmp<Foam::GeometricField<double,\s*Foam:\s*:fvPatchField,\s*Foam:\s*:volMesh>\s*>': 'tmp_volScalarField',
    r'Foam::tmp<Foam::GeometricField<vector,\s*Foam:\s*:fvPatchField,\s*Foam:\s*:volMesh>\s*>': 'tmp_volVectorField',
    r'Foam::tmp<Foam::GeometricField<tensor,\s*Foam:\s*:fvPatchField,\s*Foam:\s*:volMesh>\s*>': 'tmp_volTensorField',
    r'Foam::tmp<Foam::GeometricField<symmTensor,\s*Foam:\s*:fvPatchField,\s*Foam:\s*:volMesh>\s*>': 'tmp_volSymmTensorField',
    r'Foam::tmp<Foam::GeometricField<double,\s*Foam:\s*:fvsPatchField,\s*Foam:\s*:surfaceMesh>\s*>': 'tmp_surfaceScalarField',
    r'Foam::tmp<Foam::GeometricField<vector,\s*Foam:\s*:fvsPatchField,\s*Foam:\s*:surfaceMesh>\s*>': 'tmp_surfaceVectorField',
    r'Foam::tmp<Foam::GeometricField<tensor,\s*Foam:\s*:fvsPatchField,\s*Foam:\s*:surfaceMesh>\s*>': 'tmp_surfaceTensorField',
    r'Foam::tmp<Foam::GeometricField<symmTensor,\s*Foam:\s*:fvsPatchField,\s*Foam:\s*:surfaceMesh>\s*>': 'tmp_surfaceSymmTensorField',
    r'Foam::tmp<volScalarField\s*>': 'tmp_volScalarField',
    r'Foam::tmp<volVectorField\s*>': 'tmp_volVectorField',
    r'Foam::tmp<volTensorField\s*>': 'tmp_volTensorField',
    r'Foam::tmp<volSymmTensorField\s*>': 'tmp_volSymmTensorField',
    r'Foam::tmp<surfaceScalarField\s*>': 'tmp_surfaceScalarField',
    r'Foam::tmp<surfaceVectorField\s*>': 'tmp_surfaceVectorField',
    r'Foam::tmp<surfaceTensorField\s*>': 'tmp_surfaceTensorField',
    r'Foam::tmp<surfaceSymmTensorField\s*>': 'tmp_surfaceSymmTensorField',
    r'Foam::SolverPerformance<vector\s*>': 'SolverVectorPerformance',
    r'Foam::SolverPerformance<tensor\s*>': 'SolverTensorPerformance',
    r'Foam::SolverPerformance<symmTensor\s*>': 'SolverSymmTensorPerformance',
    
    # dimensioned types
    r'Foam::dimensioned<double>': 'dimensionedScalar',
    r'Foam::dimensioned<vector\s*>': 'dimensionedVector',
    r'Foam::dimensioned<tensor\s*>': 'dimensionedTensor',
    r'Foam::dimensioned<symmTensor\s*>': 'dimensionedSymmTensor',
    
    # fvMatrix types
    r'Foam::fvMatrix<double>': 'fvScalarMatrix',
    r'Foam::fvMatrix<vector\s*>': 'fvVectorMatrix',
    r'Foam::fvMatrix<tensor\s*>': 'fvTensorMatrix',
    r'Foam::fvMatrix<symmTensor\s*>': 'fvSymmTensorMatrix',
    
    # tmp with simple field names
    r'Foam::tmp<scalarField\s*>': 'tmp_scalarField',
    r'Foam::tmp<vectorField\s*>': 'tmp_vectorField',
    r'Foam::tmp<tensorField\s*>': 'tmp_tensorField',
    r'Foam::tmp<symmTensorField\s*>': 'tmp_symmTensorField',
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
    
    # Process all .pyi files in stubs directory
    stub_files = list(stubs_dir.glob('*.pyi'))
    
    if not stub_files:
        print(f"No stub files found in {stubs_dir}")
        return
    
    print(f"Cleaning {len(stub_files)} stub file(s) in {stubs_dir}...")
    
    for stub_file in stub_files:
        print(f"  Processing {stub_file.name}...")
        content = stub_file.read_text()
        original = content
        
        # Apply all replacements
        for pattern, replacement in TYPE_REPLACEMENTS.items():
            content = re.sub(pattern, replacement, content)
        
        if content != original:
            stub_file.write_text(content)
            print(f"    ✓ Cleaned {stub_file.name}")
        else:
            print(f"    - No changes needed for {stub_file.name}")
    
    print(f"\nStub cleaning complete!")


if __name__ == '__main__':
    main()
