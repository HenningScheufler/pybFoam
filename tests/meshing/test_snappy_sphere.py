"""
Test snappyHexMesh Python binding against native OpenFOAM implementation.

Tests each phase individually (castellated, snap, layers) to ensure the Python
binding produces identical results to the native snappyHexMesh utility.
"""

import json
import shutil
from pathlib import Path
from typing import TYPE_CHECKING

import pytest

import pybFoam.pybFoam_core as pyb

if TYPE_CHECKING:
    # Import shared utilities from conftest
    from .conftest import (
        compare_mesh_stats,
        get_mesh_stats,
        modify_snappy_dict,
        run_blockmesh,
        run_native_snappy,
        run_python_snappy,
        run_surface_feature_extract,
    )
else:
    # Import shared utilities from conftest
    from .conftest import (
        compare_mesh_stats,
        get_mesh_stats,
        modify_snappy_dict,
        run_blockmesh,
        run_native_snappy,
        run_python_snappy,
        run_surface_feature_extract,
    )


@pytest.fixture
def sphere_simple_case() -> Path:
    """Get path to the sphere_simple test case."""
    test_dir = Path(__file__).parent / "sphere_simple"
    assert test_dir.exists(), f"sphere_simple case not found at {test_dir}"
    return test_dir


@pytest.fixture
def temp_case_native(tmp_path: Path, sphere_simple_case: Path) -> Path:
    """Create a temporary copy of sphere_simple for native OpenFOAM run."""
    temp_dir = tmp_path / "native"
    shutil.copytree(sphere_simple_case, temp_dir)
    return temp_dir


@pytest.fixture
def temp_case_python(tmp_path: Path, sphere_simple_case: Path) -> Path:
    """Create a temporary copy of sphere_simple for Python binding run."""
    temp_dir = tmp_path / "python"
    shutil.copytree(sphere_simple_case, temp_dir)
    return temp_dir


@pytest.mark.parametrize(
    "castellated,snap,layers,test_name",
    [
        (True, False, False, "castellated_only"),
        (True, True, False, "castellated_and_snap"),
        (True, True, True, "all_phases"),
    ],
)
def test_snappy_phase_comparison(
    temp_case_native: Path, temp_case_python: Path, castellated: bool, snap: bool, layers: bool, test_name: str, tmp_path: Path
) -> None:
    """
    Test that Python binding produces identical results to native snappyHexMesh.

    This test runs both implementations with the same configuration and compares
    the resulting mesh statistics to ensure they match exactly.
    """
    # Modify snappyHexMeshDict for both cases
    modify_snappy_dict(temp_case_native, castellated, snap, layers)
    modify_snappy_dict(temp_case_python, castellated, snap, layers)

    # Create log directory
    log_dir = tmp_path / "logs"
    log_dir.mkdir(exist_ok=True)

    # Prepare both cases: run blockMesh and surfaceFeatureExtract
    run_blockmesh(temp_case_native, log_dir / f"{test_name}_native_blockmesh.log")
    run_surface_feature_extract(
        temp_case_native, log_dir / f"{test_name}_native_surfaceFeatureExtract.log"
    )

    run_blockmesh(temp_case_python, log_dir / f"{test_name}_python_blockmesh.log")
    run_surface_feature_extract(
        temp_case_python, log_dir / f"{test_name}_python_surfaceFeatureExtract.log"
    )

    # Run native snappyHexMesh
    run_native_snappy(temp_case_native, log_dir / f"{test_name}_native_snappyHexMesh.log")

    # Create Time objects for Python case (do this once to avoid duplicate JobInfo warnings)
    argv_python = [str(temp_case_python), "-case", str(temp_case_python)]
    arglist_python = pyb.argList(argv_python)
    time_python = pyb.Time(arglist_python)

    # Run Python snappyHexMesh
    run_python_snappy(temp_case_python, time_python)

    # Create Time objects for native analysis
    argv_native = [str(temp_case_native), "-case", str(temp_case_native)]
    arglist_native = pyb.argList(argv_native)
    time_native = pyb.Time(arglist_native)

    # Get mesh statistics from both
    native_stats = get_mesh_stats(temp_case_native, time_native)
    python_stats = get_mesh_stats(temp_case_python, time_python)

    # Save stats to JSON for debugging
    stats_dir = tmp_path / "stats"
    stats_dir.mkdir(exist_ok=True)

    with open(stats_dir / f"{test_name}_native.json", "w") as f:
        json.dump(native_stats, f, indent=2)

    with open(stats_dir / f"{test_name}_python.json", "w") as f:
        json.dump(python_stats, f, indent=2)

    # Compare statistics
    errors = compare_mesh_stats(native_stats, python_stats, tolerance=1e-6)

    if errors:
        pytest.fail("Mesh statistics do not match:\n" + "\n".join(errors))

    # Additional verification: both meshes should have the same checkMesh status
    # Note: The test mesh may have quality issues (e.g., concave cells), but both
    # implementations should produce the same quality
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
