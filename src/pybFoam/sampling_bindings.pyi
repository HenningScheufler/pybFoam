"""OpenFOAM sampling and surface functionality"""

import pybFoam.pybFoam_core


class sampledSurface:
    def name(self) -> pybFoam.pybFoam_core.Word:
        """Get the name of the surface"""

    def enabled(self) -> bool:
        """Check if surface is enabled"""

    def invariant(self) -> bool:
        """Check if surface is invariant with geometry changes"""

    def isPointData(self) -> bool:
        """Check if using interpolation to surface points"""

    def needsUpdate(self) -> bool:
        """Check if the surface needs an update"""

    def expire(self) -> bool:
        """Mark the surface as needing an update"""

    def update(self) -> bool:
        """Update the surface as required"""

    def points(self) -> pybFoam.pybFoam_core.vectorField:
        """Get points of surface"""

    def Sf(self) -> pybFoam.pybFoam_core.vectorField:
        """Get face area vectors"""

    def magSf(self) -> pybFoam.pybFoam_core.scalarField:
        """Get face area magnitudes"""

    def Cf(self) -> pybFoam.pybFoam_core.vectorField:
        """Get face centres"""

    def area(self) -> float:
        """Get total surface area"""

    def hasFaceIds(self) -> bool:
        """Check if element ids/order of original surface are available"""

    @staticmethod
    def New(name: pybFoam.pybFoam_core.Word, mesh: pybFoam.pybFoam_core.fvMesh, dict: pybFoam.pybFoam_core.dictionary) -> sampledSurface:
        """Construct a new sampledSurface from dictionary (fvMesh overload)"""

class meshSearch:
    def __init__(self, mesh: pybFoam.pybFoam_core.fvMesh) -> None:
        """Construct from fvMesh"""

    def findNearestCell(self, location: pybFoam.pybFoam_core.vector, seedCelli: int = -1, useTreeSearch: bool = True) -> int:
        """Find nearest cell to location"""

    def findCell(self, location: pybFoam.pybFoam_core.vector, seedCelli: int = -1, useTreeSearch: bool = True) -> int:
        """Find cell containing location"""

class sampledSet:
    def name(self) -> pybFoam.pybFoam_core.Word:
        """Get the name of the set"""

    def axis(self) -> pybFoam.pybFoam_core.Word:
        """Get the axis name (x, y, z, xyz, distance)"""

    def points(self) -> pybFoam.pybFoam_core.vectorField:
        """Get the sampling points"""

    def distance(self) -> pybFoam.pybFoam_core.scalarField:
        """Get cumulative distance along the set"""

    def nPoints(self) -> int:
        """Get number of points in the set"""

    def searchEngine(self) -> meshSearch:
        """Get reference to the mesh search engine"""

    def segments(self) -> pybFoam.pybFoam_core.labelList:
        """Get segment numbers for each point"""

    def cells(self) -> pybFoam.pybFoam_core.labelList:
        """Get cell IDs for each point"""

    @staticmethod
    def New(name: pybFoam.pybFoam_core.Word, mesh: pybFoam.pybFoam_core.fvMesh, searchEngine: meshSearch, dict: pybFoam.pybFoam_core.dictionary) -> sampledSet:
        """Construct a new sampledSet from dictionary"""

class interpolationScalar:
    @staticmethod
    def New(interpolationType: pybFoam.pybFoam_core.Word, field: pybFoam.pybFoam_core.volScalarField) -> interpolationScalar:
        """Create scalar interpolation scheme"""

class interpolationVector:
    @staticmethod
    def New(interpolationType: pybFoam.pybFoam_core.Word, field: pybFoam.pybFoam_core.volVectorField) -> interpolationVector:
        """Create vector interpolation scheme"""

class interpolationTensor:
    @staticmethod
    def New(interpolationType: pybFoam.pybFoam_core.Word, field: pybFoam.pybFoam_core.volTensorField) -> interpolationTensor:
        """Create tensor interpolation scheme"""

class interpolationSymmTensor:
    @staticmethod
    def New(interpolationType: pybFoam.pybFoam_core.Word, field: pybFoam.pybFoam_core.volSymmTensorField) -> interpolationSymmTensor:
        """Create symmTensor interpolation scheme"""

def sampleOnFacesScalar(surface: sampledSurface, interpolator: interpolationScalar) -> pybFoam.pybFoam_core.scalarField:
    """Sample scalar field values onto surface faces"""

def sampleOnFacesVector(surface: sampledSurface, interpolator: interpolationVector) -> pybFoam.pybFoam_core.vectorField:
    """Sample vector field values onto surface faces"""

def sampleOnFacesTensor(surface: sampledSurface, interpolator: interpolationTensor) -> pybFoam.pybFoam_core.tensorField:
    """Sample tensor field values onto surface faces"""

def sampleOnFacesSymmTensor(surface: sampledSurface, interpolator: interpolationSymmTensor) -> pybFoam.pybFoam_core.symmTensorField:
    """Sample symmTensor field values onto surface faces"""

def sampleOnPointsScalar(surface: sampledSurface, interpolator: interpolationScalar) -> pybFoam.pybFoam_core.scalarField:
    """Interpolate scalar field values onto surface points"""

def sampleOnPointsVector(surface: sampledSurface, interpolator: interpolationVector) -> pybFoam.pybFoam_core.vectorField:
    """Interpolate vector field values onto surface points"""

def sampleOnPointsTensor(surface: sampledSurface, interpolator: interpolationTensor) -> pybFoam.pybFoam_core.tensorField:
    """Interpolate tensor field values onto surface points"""

def sampleOnPointsSymmTensor(surface: sampledSurface, interpolator: interpolationSymmTensor) -> pybFoam.pybFoam_core.symmTensorField:
    """Interpolate symmTensor field values onto surface points"""

def sampleSetScalar(sampledSet: sampledSet, interpolator: interpolationScalar) -> pybFoam.pybFoam_core.scalarField:
    """Sample scalar field values onto sampledSet points"""

def sampleSetVector(sampledSet: sampledSet, interpolator: interpolationVector) -> pybFoam.pybFoam_core.vectorField:
    """Sample vector field values onto sampledSet points"""

def sampleSetTensor(sampledSet: sampledSet, interpolator: interpolationTensor) -> pybFoam.pybFoam_core.tensorField:
    """Sample tensor field values onto sampledSet points"""

def sampleSetSymmTensor(sampledSet: sampledSet, interpolator: interpolationSymmTensor) -> pybFoam.pybFoam_core.symmTensorField:
    """Sample symmTensor field values onto sampledSet points"""
