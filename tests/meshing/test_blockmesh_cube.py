"""
Simplified blockMesh test.

Tests basic blockMesh mesh generation functionality by comparing native
OpenFOAM blockMesh with Python binding implementation.
"""

import shutil
import subprocess
from pathlib import Path
from typing import Any

import pytest

import pybFoam.pybFoam_core as core
from pybFoam import meshing


@pytest.fixture
def cube_case() -> Path:
    """Get path to the existing cube test case."""
    test_dir = Path(__file__).parent
    case_path = test_dir / "cube"
    assert case_path.exists(), f"Cube case not found at {case_path}"
    return case_path


@pytest.fixture
def temp_case_native(tmp_path: Path, cube_case: Path) -> Path:
    """Create a temporary copy of cube for native OpenFOAM run."""
    temp_dir = tmp_path / "native"
    shutil.copytree(cube_case, temp_dir)
    return temp_dir


@pytest.fixture
def temp_case_python(tmp_path: Path, cube_case: Path) -> Path:
    """Create a temporary copy of cube for Python binding run."""
    temp_dir = tmp_path / "python"
    shutil.copytree(cube_case, temp_dir)
    return temp_dir


def run_native_blockmesh(case_path: Path, log_file: Path) -> subprocess.CompletedProcess[str]:
    """Run native OpenFOAM blockMesh.

    Args:
        case_path: Path to the OpenFOAM case
        log_file: Path to redirect stdout/stderr
    """
    with open(log_file, "w") as f:
        result = subprocess.run(
            ["blockMesh", "-case", str(case_path)], stdout=f, stderr=subprocess.STDOUT, text=True
        )

    if result.returncode != 0:
        raise RuntimeError(
            f"blockMesh failed with return code {result.returncode}. See log: {log_file}"
        )

    return result


def run_python_blockmesh(case_path: Path, time: core.Time) -> Any:
    """Run Python binding blockMesh."""
    # Load blockMeshDict
    dict_path = case_path / "system" / "blockMeshDict"
    if not dict_path.exists():
        dict_path = case_path / "constant" / "blockMeshDict"

    block_mesh_dict = core.dictionary.read(str(dict_path))

    # Run generate_blockmesh
    mesh = meshing.generate_blockmesh(time, block_mesh_dict)

    return mesh


def get_mesh_stats(case_path: Path, time: core.Time) -> Any:
    """Get mesh statistics using checkMesh binding."""
    # Load mesh
    mesh = core.fvMesh(time)

    # Run checkMesh
    result = meshing.checkMesh(
        mesh, check_topology=True, all_topology=True, all_geometry=True, check_quality=False
    )

    return result


def test_blockmesh_comparison(
    temp_case_native: Path, temp_case_python: Path, tmp_path: Path
) -> None:
    """Test that Python binding produces identical results to native blockMesh."""

    native_log = tmp_path / "native_blockmesh.log"

    run_native_blockmesh(temp_case_native, native_log)

    argv_python = [str(temp_case_python), "-case", str(temp_case_python)]
    arglist_python = core.argList(argv_python)
    time_python = core.Time(arglist_python)

    run_python_blockmesh(temp_case_python, time_python)

    argv_native = [str(temp_case_native), "-case", str(temp_case_native)]
    arglist_native = core.argList(argv_native)
    time_native = core.Time(arglist_native)

    native_stats = get_mesh_stats(temp_case_native, time_native)
    python_stats = get_mesh_stats(temp_case_python, time_python)

    # Compare mesh statistics
    assert native_stats["mesh_stats"]["cells"] == python_stats["mesh_stats"]["cells"], (
        f"Cell count mismatch: native={native_stats['mesh_stats']['cells']}, "
        f"python={python_stats['mesh_stats']['cells']}"
    )

    assert native_stats["mesh_stats"]["faces"] == python_stats["mesh_stats"]["faces"], (
        f"Face count mismatch: native={native_stats['mesh_stats']['faces']}, "
        f"python={python_stats['mesh_stats']['faces']}"
    )

    assert native_stats["mesh_stats"]["points"] == python_stats["mesh_stats"]["points"], (
        f"Point count mismatch: native={native_stats['mesh_stats']['points']}, "
        f"python={python_stats['mesh_stats']['points']}"
    )

    assert (
        native_stats["mesh_stats"]["internal_faces"] == python_stats["mesh_stats"]["internal_faces"]
    ), (
        f"Internal face count mismatch: "
        f"native={native_stats['mesh_stats']['internal_faces']}, "
        f"python={python_stats['mesh_stats']['internal_faces']}"
    )

    # Verify both meshes pass checkMesh with same status
    if native_stats["passed"] != python_stats["passed"]:
        pytest.fail(
            f"CheckMesh status differs:\n"
            f"  Native: {'PASSED' if native_stats['passed'] else 'FAILED'}\n"
            f"  Python: {'PASSED' if python_stats['passed'] else 'FAILED'}"
        )

    if native_stats["total_errors"] != python_stats["total_errors"]:
        pytest.fail(
            f"CheckMesh error count differs:\n"
            f"  Native: {native_stats['total_errors']} errors\n"
            f"  Python: {python_stats['total_errors']} errors"
        )

    assert native_stats["mesh_stats"]["cells"] == 8000, (
        f"Expected 8000 cells, got {native_stats['mesh_stats']['cells']}"
    )
    assert native_stats["mesh_stats"]["points"] == 9261, (
        f"Expected 9261 points, got {native_stats['mesh_stats']['points']}"
    )
    assert native_stats["mesh_stats"]["faces"] == 25200, (
        f"Expected 25200 faces, got {native_stats['mesh_stats']['faces']}"
    )
    assert native_stats["mesh_stats"]["internal_faces"] == 22800, (
        f"Expected 22800 internal faces, got {native_stats['mesh_stats']['internal_faces']}"
    )
