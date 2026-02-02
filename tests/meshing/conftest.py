"""
Shared fixtures and utilities for meshing tests.

This module contains common test utilities to avoid duplication between
test_snappy_sphere.py and test_snappy_motorbike.py.
"""

import subprocess
from pathlib import Path
from typing import Any, Dict

import pybFoam.bind_checkmesh as checkmesh
import pybFoam.meshing as meshing
import pybFoam.pybFoam_core as pyb


def modify_snappy_dict(
    case_path: Path, castellated: bool = True, snap: bool = True, layers: bool = False
) -> None:
    """
    Modify snappyHexMeshDict to enable/disable specific phases.

    Args:
        case_path: Path to the OpenFOAM case
        castellated: Enable castellatedMesh phase
        snap: Enable snap phase
        layers: Enable addLayers phase
    """
    dict_path = case_path / "system" / "snappyHexMeshDict"
    assert dict_path.exists(), f"snappyHexMeshDict not found at {dict_path}"

    # Read the dictionary
    with open(dict_path, "r") as f:
        content = f.read()

    # Replace phase settings
    content = content.replace(
        "castellatedMesh true;", f"castellatedMesh {str(castellated).lower()};"
    )
    content = content.replace(
        "castellatedMesh false;", f"castellatedMesh {str(castellated).lower()};"
    )
    content = content.replace("snap true;", f"snap {str(snap).lower()};")
    content = content.replace("snap false;", f"snap {str(snap).lower()};")
    content = content.replace("addLayers true;", f"addLayers {str(layers).lower()};")
    content = content.replace("addLayers false;", f"addLayers {str(layers).lower()};")

    # Write back
    with open(dict_path, "w") as f:
        f.write(content)


def run_blockmesh(case_path: Path, log_file=None) -> None:
    """Run blockMesh to generate background mesh."""
    if log_file:
        with open(log_file, "w") as f:
            result = subprocess.run(
                ["blockMesh", "-case", str(case_path)],
                stdout=f,
                stderr=subprocess.STDOUT,
                text=True,
            )
    else:
        result = subprocess.run(
            ["blockMesh", "-case", str(case_path)], capture_output=True, text=True, check=True
        )
    assert result.returncode == 0, "blockMesh failed"


def run_surface_feature_extract(case_path: Path, log_file=None) -> None:
    """Run surfaceFeatureExtract if needed."""
    # Check if eMesh files already exist
    constant_dir = case_path / "constant"
    emesh_files = list(constant_dir.glob("**/*.eMesh"))

    if not emesh_files:
        if log_file:
            with open(log_file, "w") as f:
                result = subprocess.run(
                    ["surfaceFeatureExtract", "-case", str(case_path)],
                    stdout=f,
                    stderr=subprocess.STDOUT,
                    text=True,
                )
        else:
            result = subprocess.run(
                ["surfaceFeatureExtract", "-case", str(case_path)], capture_output=True, text=True
            )
        # Note: surfaceFeatureExtract may return non-zero even on success
        # so we don't check returncode strictly


def run_native_snappy(case_path: Path, log_file=None) -> Any:
    """Run native OpenFOAM snappyHexMesh."""
    if log_file:
        with open(log_file, "w") as f:
            result = subprocess.run(
                ["snappyHexMesh", "-overwrite", "-case", str(case_path)],
                stdout=f,
                stderr=subprocess.STDOUT,
                text=True,
            )
    else:
        result = subprocess.run(
            ["snappyHexMesh", "-overwrite", "-case", str(case_path)], capture_output=True, text=True
        )

    if result.returncode != 0:
        raise RuntimeError(f"snappyHexMesh failed with return code {result.returncode}")

    return result


def run_python_snappy(case_path: Path, time: pyb.Time) -> pyb.fvMesh:
    """Run Python binding snappyHexMesh."""
    # Load mesh
    mesh = pyb.fvMesh(time)

    # Load snappyHexMeshDict
    dict_path = case_path / "system" / "snappyHexMeshDict"
    snappy_dict = pyb.dictionary.read(str(dict_path))

    # Run snappyHexMesh
    meshing.generate_snappy_hex_mesh(mesh=mesh, dict=snappy_dict, overwrite=True, verbose=False)

    # Write mesh
    mesh.write()

    return mesh


def get_mesh_stats(case_path: Path, time: pyb.Time) -> Dict[str, Any]:
    """
    Get mesh statistics using checkMesh binding.

    Returns a dictionary with mesh statistics that can be compared.
    """
    # Load mesh
    mesh = pyb.fvMesh(time)

    # Run checkMesh with all options (output is expected here)
    result = checkmesh.checkMesh(
        mesh, check_topology=True, all_topology=True, all_geometry=True, check_quality=False
    )

    return result


def compare_mesh_stats(
    native_stats: Dict[str, Any], python_stats: Dict[str, Any], tolerance: float = 1e-6
) -> list[str]:
    """
    Compare mesh statistics from native and Python implementations.

    Args:
        native_stats: Statistics from native OpenFOAM
        python_stats: Statistics from Python binding
        tolerance: Relative tolerance for floating point comparisons

    Returns:
        List of error messages if differences found, empty list otherwise
    """
    errors = []

    # Compare exact integer counts
    exact_keys = [
        ("mesh_stats", "cells"),
        ("mesh_stats", "faces"),
        ("mesh_stats", "internal_faces"),
        ("mesh_stats", "points"),
        ("mesh_stats", "boundary_patches"),
        ("cell_types", "hexahedra"),
        ("cell_types", "prisms"),
        ("cell_types", "wedges"),
        ("cell_types", "pyramids"),
        ("cell_types", "tetrahedra"),
        ("cell_types", "polyhedra"),
    ]

    for section, key in exact_keys:
        native_val = native_stats.get(section, {}).get(key)
        python_val = python_stats.get(section, {}).get(key)

        if native_val is not None and python_val is not None:
            if native_val != python_val:
                errors.append(f"{section}.{key}: native={native_val}, python={python_val}")

    # Compare floating point values with tolerance
    float_keys = [
        ("geometry", "min_volume"),
        ("geometry", "max_volume"),
        ("geometry", "total_volume"),
        ("geometry", "min_face_area"),
        ("geometry", "max_face_area"),
        ("geometry", "min_edge_length"),
        ("geometry", "max_edge_length"),
    ]

    for section, key in float_keys:
        native_val = native_stats.get(section, {}).get(key)
        python_val = python_stats.get(section, {}).get(key)

        if native_val is not None and python_val is not None:
            if abs(native_val) > tolerance:
                rel_diff = abs(native_val - python_val) / abs(native_val)
            else:
                rel_diff = abs(native_val - python_val)

            if rel_diff > tolerance:
                errors.append(
                    f"{section}.{key}: native={native_val:.6e}, python={python_val:.6e}, "
                    f"rel_diff={rel_diff:.6e}"
                )

    return errors
