"""Type stubs for mesh_generation module."""

from typing import Literal
from pybFoam.pybFoam_core import dictionary, Time, polyMesh

def generate_blockmesh(
    runtime: Time,
    blockmesh_dict: dictionary,
    merge_strategy: Literal["topology", "points"] = "topology",
    verbose: bool = False,
    time_name: str = "constant",
    no_clean: bool = False
) -> polyMesh:
    """
    Generate a block mesh from dictionary specification and return polyMesh.
    
    Parameters
    ----------
    runtime : Time
        OpenFOAM Time object for the case.
    blockmesh_dict : dictionary
        OpenFOAM dictionary object matching blockMeshDict format.
        Should contain entries for: vertices, blocks, edges (optional),
        boundary, mergePatchPairs (optional), defaultPatch (optional).
    merge_strategy : str, optional
        Merge strategy: "topology" (default) or "points".
        Topology merge is faster and works for most cases.
    verbose : bool, optional
        Enable OpenFOAM output messages (default: False).
    time_name : str, optional
        Time directory name for mesh output (default: "constant").
    no_clean : bool, optional
        Do not remove old polyMesh directory (default: False).
    
    Returns
    -------
    polyMesh
        The generated OpenFOAM polyMesh object.
        Can be used with other pybFoam functions and methods.
        Access mesh properties: nPoints(), nCells(), nFaces(), etc.
    
    Raises
    ------
    RuntimeError
        If mesh generation fails or invalid parameters provided.
    """
    ...

__all__ = ["generate_blockmesh"]
