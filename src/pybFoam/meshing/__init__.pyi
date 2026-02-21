"""OpenFOAM mesh checking and validation utilities"""

import typing
from typing import overload

import pybFoam.pybFoam_core


def generate_blockmesh(runtime: pybFoam.pybFoam_core.Time, blockmesh_dict: pybFoam.pybFoam_core.dictionary, verbose: bool = False, time_name: str = 'constant') -> pybFoam.pybFoam_core.fvMesh:
    """
    Generate a block mesh from dictionary and return fvMesh.

    Parameters
    ----------
    runtime : Time
        OpenFOAM Time object for the case.
    blockmesh_dict : dictionary
        OpenFOAM dictionary with blockMeshDict format.
        Should contain: vertices, blocks, boundary.
    verbose : bool, optional
        Enable OpenFOAM output messages (default: False).
    time_name : str, optional
        Time directory for mesh output (default: "constant").

    Returns
    -------
    fvMesh
        The generated OpenFOAM fvMesh object.

    Raises
    ------
    RuntimeError
        If mesh generation fails.

    Examples
    -------->>> import pybFoam.mesh_generation as mg>>> from pybFoam.core import Time, dictionary>>>>>> # Create Time and dictionary>>> time = Time("/path/to/case")>>> mesh_dict = dictionary()>>>>>> # Generate mesh>>> mesh = mg.generate_blockmesh(time, mesh_dict, verbose=True)>>> print(f"Generated {mesh.nCells()} cells")
    """

@overload
def printMeshStats(mesh: pybFoam.pybFoam_core.polyMesh, all_topology: bool = False) -> dict[str, typing.Any]:
    """Print mesh statistics and return as dictionary"""

@overload
def printMeshStats(mesh: pybFoam.pybFoam_core.fvMesh, all_topology: bool = False) -> dict[str, typing.Any]: ...

@overload
def checkTopology(mesh: pybFoam.pybFoam_core.polyMesh, all_topology: bool = False, all_geometry: bool = False) -> dict[str, typing.Any]:
    """Check mesh topology and return dictionary with results"""

@overload
def checkTopology(mesh: pybFoam.pybFoam_core.fvMesh, all_topology: bool = False, all_geometry: bool = False) -> dict[str, typing.Any]: ...

@overload
def checkGeometry(mesh: pybFoam.pybFoam_core.polyMesh, all_geometry: bool = False) -> dict[str, typing.Any]:
    """Check mesh geometry and return dictionary with results"""

@overload
def checkGeometry(mesh: pybFoam.pybFoam_core.fvMesh, all_geometry: bool = False) -> dict[str, typing.Any]: ...

@overload
def checkMesh(mesh: pybFoam.pybFoam_core.polyMesh, check_topology: bool = True, all_topology: bool = False, all_geometry: bool = False, check_quality: bool = False) -> dict[str, typing.Any]:
    """Run complete mesh check and return dictionary with detailed results"""

@overload
def checkMesh(mesh: pybFoam.pybFoam_core.fvMesh, check_topology: bool = True, all_topology: bool = False, all_geometry: bool = False, check_quality: bool = False) -> dict[str, typing.Any]: ...

def generate_snappy_hex_mesh(mesh: pybFoam.pybFoam_core.fvMesh, dict: pybFoam.pybFoam_core.dictionary, overwrite: bool = True, verbose: bool = True) -> None:
    """Run snappyHexMesh on an existing mesh"""
