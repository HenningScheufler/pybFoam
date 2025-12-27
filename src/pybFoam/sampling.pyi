"""
Type stubs for pybFoam.sampling module

This module provides Python bindings for OpenFOAM's sampledSurface functionality,
allowing surface creation, field interpolation, and surface sampling operations.
"""

from typing import overload
from pybFoam import (
    fvMesh,
    polyMesh,
    vectorField,
    scalarField,
    tensorField,
    symmTensorField,
    pointField,
    faceList,
    dictionary,
    word,
    volScalarField,
    volVectorField,
    volTensorField,
    volSymmTensorField,
)

class sampledSurface:
    """
    Base class for surfaces with sampling capabilities.
    
    This is an abstract base class that provides the interface for
    different types of sampled surfaces (planes, patches, etc.).
    """
    
    def name(self) -> word:
        """Get the name of the surface."""
        ...
    
    def mesh(self) -> polyMesh:
        """Get reference to the mesh."""
        ...
    
    def enabled(self) -> bool:
        """Check if surface is enabled."""
        ...
    
    def invariant(self) -> bool:
        """Check if surface is invariant with geometry changes."""
        ...
    
    def isPointData(self) -> bool:
        """Check if using interpolation to surface points."""
        ...
    
    def needsUpdate(self) -> bool:
        """Check if the surface needs an update."""
        ...
    
    def expire(self) -> bool:
        """Mark the surface as needing an update."""
        ...
    
    def update(self) -> bool:
        """Update the surface as required. Returns True if updated."""
        ...
    
    def points(self) -> pointField:
        """Get points of surface."""
        ...
    
    def faces(self) -> faceList:
        """Get faces of surface."""
        ...
    
    def Sf(self) -> vectorField:
        """Get face area vectors."""
        ...
    
    def magSf(self) -> scalarField:
        """Get face area magnitudes."""
        ...
    
    def Cf(self) -> vectorField:
        """Get face centres."""
        ...
    
    def area(self) -> float:
        """Get total surface area."""
        ...
    
    def hasFaceIds(self) -> bool:
        """Check if element ids/order of original surface are available."""
        ...
    
    @staticmethod
    def New(name: word, mesh: polyMesh, dict: dictionary) -> sampledSurface:
        """Construct a new sampledSurface from dictionary."""
        ...


class sampledPlane(sampledSurface):
    """
    A plane surface for sampling.
    
    Creates a plane surface defined by a base point and normal vector.
    """
    
    def __init__(self, name: word, mesh: polyMesh, dict: dictionary) -> None:
        """
        Construct sampledPlane from name, mesh and dictionary.
        
        Dictionary should contain:
            - type: "plane"
            - basePoint: vector defining a point on the plane
            - normalVector: vector defining the plane normal
        """
        ...


class sampledPatch(sampledSurface):
    """
    A surface created from one or more mesh patches.
    
    Samples directly from specified boundary patches.
    """
    
    def __init__(self, name: word, mesh: polyMesh, dict: dictionary) -> None:
        """
        Construct sampledPatch from name, mesh and dictionary.
        
        Dictionary should contain:
            - type: "patch"
            - patches: list of patch names (wordList)
        """
        ...


class sampledCuttingPlane(sampledSurface):
    """
    A cutting plane surface for sampling.
    
    Similar to sampledPlane but uses a different algorithm for
    cutting through the mesh.
    """
    
    def __init__(self, name: word, mesh: polyMesh, dict: dictionary) -> None:
        """
        Construct sampledCuttingPlane from name, mesh and dictionary.
        
        Dictionary should contain:
            - type: "cuttingPlane"
            - basePoint: vector defining a point on the plane
            - normalVector: vector defining the plane normal
        """
        ...


class interpolationScalar:
    """Interpolation scheme for scalar fields."""
    
    @staticmethod
    def New(interpolationType: word, field: volScalarField) -> interpolationScalar:
        """
        Create scalar interpolation scheme.
        
        Args:
            interpolationType: Type of interpolation ("cell", "cellPoint", "cellPointFace")
            field: Volume scalar field to interpolate
        """
        ...


class interpolationVector:
    """Interpolation scheme for vector fields."""
    
    @staticmethod
    def New(interpolationType: word, field: volVectorField) -> interpolationVector:
        """
        Create vector interpolation scheme.
        
        Args:
            interpolationType: Type of interpolation ("cell", "cellPoint", "cellPointFace")
            field: Volume vector field to interpolate
        """
        ...


class interpolationTensor:
    """Interpolation scheme for tensor fields."""
    
    @staticmethod
    def New(interpolationType: word, field: volTensorField) -> interpolationTensor:
        """
        Create tensor interpolation scheme.
        
        Args:
            interpolationType: Type of interpolation ("cell", "cellPoint", "cellPointFace")
            field: Volume tensor field to interpolate
        """
        ...


class interpolationSymmTensor:
    """Interpolation scheme for symmetric tensor fields."""
    
    @staticmethod
    def New(interpolationType: word, field: volSymmTensorField) -> interpolationSymmTensor:
        """
        Create symmTensor interpolation scheme.
        
        Args:
            interpolationType: Type of interpolation ("cell", "cellPoint", "cellPointFace")
            field: Volume symmTensor field to interpolate
        """
        ...


# Sampling functions - sample onto surface faces

def sampleOnFacesScalar(surface: sampledSurface, interpolator: interpolationScalar) -> scalarField:
    """Sample scalar field values onto surface faces."""
    ...


def sampleOnFacesVector(surface: sampledSurface, interpolator: interpolationVector) -> vectorField:
    """Sample vector field values onto surface faces."""
    ...


def sampleOnFacesTensor(surface: sampledSurface, interpolator: interpolationTensor) -> tensorField:
    """Sample tensor field values onto surface faces."""
    ...


def sampleOnFacesSymmTensor(surface: sampledSurface, interpolator: interpolationSymmTensor) -> symmTensorField:
    """Sample symmTensor field values onto surface faces."""
    ...


# Sampling functions - interpolate to surface points

def sampleOnPointsScalar(surface: sampledSurface, interpolator: interpolationScalar) -> scalarField:
    """Interpolate scalar field values onto surface points."""
    ...


def sampleOnPointsVector(surface: sampledSurface, interpolator: interpolationVector) -> vectorField:
    """Interpolate vector field values onto surface points."""
    ...


def sampleOnPointsTensor(surface: sampledSurface, interpolator: interpolationTensor) -> tensorField:
    """Interpolate tensor field values onto surface points."""
    ...


def sampleOnPointsSymmTensor(surface: sampledSurface, interpolator: interpolationSymmTensor) -> symmTensorField:
    """Interpolate symmTensor field values onto surface points."""
    ...
