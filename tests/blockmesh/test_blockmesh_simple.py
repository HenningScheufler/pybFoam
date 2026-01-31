"""
Simplified blockMesh test.

Tests basic blockMesh mesh generation functionality.
"""

import pytest
import subprocess
from pathlib import Path

import pybFoam.pybFoam_core as core


@pytest.fixture
def cube_case():
    """Get path to the existing cube test case."""
    test_dir = Path(__file__).parent
    case_path = test_dir / "cube"
    assert case_path.exists(), f"Cube case not found at {case_path}"
    return case_path


def test_blockmesh_simple_cube(cube_case):
    """Test generating a simple cube mesh with blockMesh and verify polyMesh output."""
    
    # Clean any existing mesh
    poly_mesh_path = cube_case / "constant" / "polyMesh"
    if poly_mesh_path.exists():
        import shutil
        shutil.rmtree(poly_mesh_path)
    
    # Run blockMesh to generate the mesh
    result = subprocess.run(
        ["blockMesh", "-case", str(cube_case)],
        capture_output=True,
        text=True
    )
    
    # Check that blockMesh succeeded
    assert result.returncode == 0, f"blockMesh failed: {result.stderr}"
    
    # Verify mesh files exist
    assert poly_mesh_path.exists(), "polyMesh directory not created"
    assert (poly_mesh_path / "points").exists(), "points file not created"
    assert (poly_mesh_path / "faces").exists(), "faces file not created"
    assert (poly_mesh_path / "owner").exists(), "owner file not created"
    assert (poly_mesh_path / "neighbour").exists(), "neighbour file not created"
    assert (poly_mesh_path / "boundary").exists(), "boundary file not created"
    
    # Load the mesh using Python bindings
    argv = [str(cube_case), "-case", str(cube_case)]
    arglist = core.argList(argv)
    time_obj = core.Time(arglist)
    mesh = core.fvMesh(time_obj)
    
    # Verify mesh properties (20x20x20 = 8000 cells from blockMeshDict)
    print(f"\nMesh Statistics:")
    print(f"  Cells:      {mesh.nCells()}")
    print(f"  Points:     {mesh.nPoints()}")
    print(f"  Faces:      {mesh.nFaces()}")
    print(f"  Int. Faces: {mesh.nInternalFaces()}")
    print(f"  Boundaries: {mesh.boundary().size()}")
    
    # Validate against expected values for 20x20x20 cube
    assert mesh.nCells() == 8000, f"Expected 8000 cells, got {mesh.nCells()}"
    assert mesh.nPoints() == 9261, f"Expected 9261 points, got {mesh.nPoints()}"
    assert mesh.nFaces() == 25200, f"Expected 25200 faces, got {mesh.nFaces()}"
    assert mesh.nInternalFaces() == 22800, f"Expected 22800 internal faces, got {mesh.nInternalFaces()}"
    
    # Check boundary patches
    boundary = mesh.boundary()
    assert boundary.size() == 3, f"Expected 3 boundary patches, got {boundary.size()}"
    
    # Verify boundary patch names and face counts
    expected_patches = {
        "movingWall": 400,      # 20x20 faces
        "fixedWalls": 1200,     # 4 sides with 300 faces each
        "frontAndBack": 800     # front and back with 400 faces each
    }
    
    for patch_name, expected_count in expected_patches.items():
        patch_id = boundary.findPatchID(patch_name)
        assert patch_id >= 0, f"Patch '{patch_name}' not found"
        patch = boundary[patch_id]
        actual_count = patch.size()
        assert actual_count == expected_count, \
            f"Patch '{patch_name}' expected {expected_count} faces, got {actual_count}"
        print(f"  Patch '{patch_name}': {actual_count} faces - OK")
    
    # Verify total boundary faces
    boundary_faces = sum(boundary[i].size() for i in range(boundary.size()))
    assert boundary_faces == mesh.nFaces() - mesh.nInternalFaces(), \
        f"Boundary face count mismatch: {boundary_faces} vs {mesh.nFaces() - mesh.nInternalFaces()}"
    
    print(f"\n✓ All mesh properties verified successfully")
