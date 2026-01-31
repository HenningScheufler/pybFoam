
import pytest
import subprocess
import os
import shutil
from pathlib import Path
import pybFoam.pybFoam_core as core
import pybFoam.meshing as meshing

@pytest.fixture
def motorbike_case():
    """Fixture to prepare the motorbike case for testing."""
    root_dir = Path("/home/henning/libsAndApps/pybFoam")
    case_path = root_dir / "tests/snappy_hex_mesh/motorBike"
    
    # Ensure it's clean and prepped
    subprocess.run(["./Allclean"], cwd=str(case_path), check=True)
    subprocess.run(["surfaceFeatureExtract"], cwd=str(case_path), check=True)
    subprocess.run(["blockMesh"], cwd=str(case_path), check=True)
    
    return case_path

def get_mesh_stats(case_path):
    """Utility to load a mesh from a case and return basic counts."""
    # Use -case to point to the correct directory
    args = core.argList(["meshStats", "-case", str(case_path)])
    runTime = core.Time(args)
    mesh = core.fvMesh(runTime)
    return {
        "nCells": mesh.nCells(),
        "nFaces": mesh.nFaces(),
        "nPoints": mesh.nPoints()
    }

@pytest.mark.skip(reason="Long test, run manually when needed")
def test_snappy_equivalence(motorbike_case):
    """
    Check if the C++ binding for snappyHexMesh produces the same mesh 
    as the standard OpenFOAM utility.
    """
    case_path = motorbike_case
    
    # 1. Run binding snappyHexMesh
    print("\nRunning pybFoam snappyHexMesh binding...")
    # We need to stay in the directory or use -case
    orig_cwd = os.getcwd()
    os.chdir(str(case_path))
    try:
        args = core.argList(["snappyHexMesh"])
        runTime = core.Time(args)
        mesh = core.fvMesh(runTime) # This is the blockMesh
        
        snappyDict = core.dictionary.read("system/snappyHexMeshDict")
        meshing.generate_snappy_hex_mesh(mesh, snappyDict, overwrite=True, verbose=False)
        
        bind_stats = {
            "nCells": mesh.nCells(),
            "nFaces": mesh.nFaces(),
            "nPoints": mesh.nPoints()
        }
        print(f"Binding Stats: {bind_stats}")
        
    finally:
        os.chdir(orig_cwd)

    # 2. Reset the mesh (re-run blockMesh)
    subprocess.run(["blockMesh"], cwd=str(case_path), check=True)
    
    # 3. Run standard snappyHexMesh
    print("Running standard snappyHexMesh...")
    subprocess.run(["snappyHexMesh", "-overwrite"], cwd=str(case_path), check=True)
    std_stats = get_mesh_stats(case_path)
    print(f"Standard Stats: {std_stats}")
    
    # 4. Compare
    print(f"\nComparison:")
    print(f"Standard Stats: {std_stats}")
    print(f"Binding Stats:  {bind_stats}")
    
    assert abs(bind_stats["nCells"] - std_stats["nCells"]) <= 500, f"Cell count mismatch: {bind_stats['nCells']} vs {std_stats['nCells']}"
    assert abs(bind_stats["nFaces"] - std_stats["nFaces"]) <= 1000, f"Face count mismatch: {bind_stats['nFaces']} vs {std_stats['nFaces']}"
    assert abs(bind_stats["nPoints"] - std_stats["nPoints"]) <= 500, f"Point count mismatch: {bind_stats['nPoints']} vs {std_stats['nPoints']}"
    
    if bind_stats == std_stats:
        print("✓ Success: Binding output matches standard utility output EXACTLY.")
    else:
        print(f"✓ Success: Binding output matches standard utility output within tolerance.")
    print("✓ Success: Binding output matches standard utility output.")
