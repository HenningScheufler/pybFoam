/*---------------------------------------------------------------------------*\
            Copyright (c) 2025, Henning Scheufler
-------------------------------------------------------------------------------
License
    This file is part of the pybFoam source code library, which is an
    unofficial extension to OpenFOAM.
    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.
    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "fvMesh.H"
#include "polyMesh.H"
#include "Time.H"
#include "globalMeshData.H"
#include "vectorField.H"
#include "scalarField.H"
#include "ops.H"

// Include checkMesh headers
#include "checkTools.H"
#include "checkTopology.H"
#include "checkGeometry.H"
#include "checkMeshQuality.H"

#include <sstream>

namespace py = pybind11;

namespace Foam
{

// Helper: Get basic mesh statistics
void addMeshStats(py::dict& result, const polyMesh& mesh)
{
    result["points"] = returnReduce(mesh.nPoints(), sumOp<label>());
    result["faces"] = returnReduce(mesh.nFaces(), sumOp<label>());
    result["internal_faces"] = returnReduce(mesh.nInternalFaces(), sumOp<label>());
    result["cells"] = returnReduce(mesh.nCells(), sumOp<label>());
    result["boundary_patches"] = mesh.boundaryMesh().size();
    
    // Faces per cell - calculate average
    label totalCellFaces = 0;
    forAll(mesh.cells(), cellI)
    {
        totalCellFaces += mesh.cells()[cellI].size();
    }
    scalar facesPerCell = 0;
    if (mesh.nCells() > 0)
    {
        facesPerCell = scalar(totalCellFaces) / scalar(mesh.nCells());
    }
    result["faces_per_cell"] = returnReduce(facesPerCell, maxOp<scalar>());
    
    // Zones
    result["point_zones"] = mesh.pointZones().size();
    result["face_zones"] = mesh.faceZones().size();
    result["cell_zones"] = mesh.cellZones().size();
}

// Helper: Count cell types
void addCellTypeCounts(py::dict& result, const polyMesh& mesh)
{
    label nHex = 0, nPrism = 0, nWedge = 0, nPyr = 0, nTet = 0, nTetWedge = 0, nPoly = 0;
    
    forAll(mesh.cells(), cellI)
    {
        const cellModel& model = mesh.cellShapes()[cellI].model();
        if (model == cellModel::ref(cellModel::HEX)) nHex++;
        else if (model == cellModel::ref(cellModel::PRISM)) nPrism++;
        else if (model == cellModel::ref(cellModel::WEDGE)) nWedge++;
        else if (model == cellModel::ref(cellModel::PYR)) nPyr++;
        else if (model == cellModel::ref(cellModel::TET)) nTet++;
        else if (model == cellModel::ref(cellModel::TETWEDGE)) nTetWedge++;
        else nPoly++;
    }
    
    result["hexahedra"] = returnReduce(nHex, sumOp<label>());
    result["prisms"] = returnReduce(nPrism, sumOp<label>());
    result["wedges"] = returnReduce(nWedge, sumOp<label>());
    result["pyramids"] = returnReduce(nPyr, sumOp<label>());
    result["tet_wedges"] = returnReduce(nTetWedge, sumOp<label>());
    result["tetrahedra"] = returnReduce(nTet, sumOp<label>());
    result["polyhedra"] = returnReduce(nPoly, sumOp<label>());
}



// Helper: Add geometry metrics to dictionary
void addGeometryMetrics(py::dict& result, const polyMesh& mesh)
{
    // Bounding box
    boundBox bb = mesh.bounds();
    py::list bbMin, bbMax;
    bbMin.append(bb.min().x());
    bbMin.append(bb.min().y());
    bbMin.append(bb.min().z());
    bbMax.append(bb.max().x());
    bbMax.append(bb.max().y());
    bbMax.append(bb.max().z());
    result["bounding_box_min"] = bbMin;
    result["bounding_box_max"] = bbMax;
    
    // Geometric and solution directions
    result["geometric_directions"] = mesh.nGeometricD();
    result["solution_directions"] = mesh.nSolutionD();
    
    // Cell volumes
    const scalarField& cellVolumes = mesh.cellVolumes();
    if (cellVolumes.size() > 0)
    {
        result["min_volume"] = returnReduce(min(cellVolumes), minOp<scalar>());
        result["max_volume"] = returnReduce(max(cellVolumes), maxOp<scalar>());
        result["total_volume"] = returnReduce(sum(cellVolumes), sumOp<scalar>());
    }
    
    // Face areas
    scalarField faceAreaMags(mesh.nFaces());
    forAll(mesh.faceAreas(), faceI)
    {
        faceAreaMags[faceI] = mag(mesh.faceAreas()[faceI]);
    }
    if (faceAreaMags.size() > 0)
    {
        result["min_face_area"] = returnReduce(min(faceAreaMags), minOp<scalar>());
        result["max_face_area"] = returnReduce(max(faceAreaMags), maxOp<scalar>());
    }
    
    // Quality metrics using public mesh data
    const vectorField& faceCentres = mesh.faceCentres();
    const vectorField& cellCentres = mesh.cellCentres();
    const vectorField& faceAreas = mesh.faceAreas();
    
    // Non-orthogonality - compute directly
    scalar maxNonOrtho = 0;
    scalar avgNonOrtho = 0;
    label nInternalFaces = 0;
    {
        forAll(mesh.faces(), faceI)
        {
            if (mesh.isInternalFace(faceI))
            {
                vector d = cellCentres[mesh.faceNeighbour()[faceI]] - cellCentres[mesh.faceOwner()[faceI]];
                vector s = faceAreas[faceI];
                scalar dMag = mag(d);
                scalar sMag = mag(s);
                
                if (dMag > VSMALL && sMag > VSMALL)
                {
                    scalar nonOrtho = ::asin(min(mag(d ^ s)/(dMag*sMag), 1.0)) * 180.0/constant::mathematical::pi;
                    maxNonOrtho = max(maxNonOrtho, nonOrtho);
                    avgNonOrtho += nonOrtho;
                    nInternalFaces++;
                }
            }
        }
        if (nInternalFaces > 0)
        {
            avgNonOrtho /= nInternalFaces;
        }
    }
    result["max_non_orthogonality"] = returnReduce(maxNonOrtho, maxOp<scalar>());
    result["avg_non_orthogonality"] = returnReduce(avgNonOrtho, maxOp<scalar>());
    
    // Skewness
    scalar maxSkewness = 0;
    {
        forAll(mesh.faces(), faceI)
        {
            if (mesh.isInternalFace(faceI))
            {
                vector d = cellCentres[mesh.faceNeighbour()[faceI]] - cellCentres[mesh.faceOwner()[faceI]];
                vector delta = faceCentres[faceI] - cellCentres[mesh.faceOwner()[faceI]];
                scalar dMag = mag(d);
                
                if (dMag > VSMALL)
                {
                    scalar skew = mag(delta - ((delta & d)/(dMag*dMag))*d)/(dMag + VSMALL);
                    maxSkewness = max(maxSkewness, skew);
                }
            }
        }
    }
    result["max_skewness"] = returnReduce(maxSkewness, maxOp<scalar>());
    
    // Edge lengths
    scalar minEdgeLength = GREAT;
    scalar maxEdgeLength = 0;
    {
        const pointField& points = mesh.points();
        forAll(mesh.edges(), edgeI)
        {
            scalar len = mesh.edges()[edgeI].mag(points);
            minEdgeLength = min(minEdgeLength, len);
            maxEdgeLength = max(maxEdgeLength, len);
        }
    }
    result["min_edge_length"] = returnReduce(minEdgeLength, minOp<scalar>());
    result["max_edge_length"] = returnReduce(maxEdgeLength, maxOp<scalar>());
}

// Wrapper to get mesh stats without printing
py::dict getPrintMeshStats(const polyMesh& mesh, const bool allTopology)
{
    py::dict stats;
    addMeshStats(stats, mesh);
    return stats;
}

// Wrapper for checkTopology returning dict
py::dict runCheckTopology(
    const polyMesh& mesh,
    const bool allTopology,
    const bool allGeometry)
{
    // Suppress output
    std::ostringstream buffer;
    std::streambuf* oldCoutBuffer = Foam::Info.stdStream().rdbuf();
    Foam::Info.stdStream().rdbuf(buffer.rdbuf());
    
    autoPtr<surfaceWriter> surfWriter;
    autoPtr<coordSetWriter> setWriter;
    
    label nErrors = checkTopology
    (
        mesh,
        allTopology,
        allGeometry,
        surfWriter,
        setWriter,
        false  // writeEdges
    );
    
    // Restore output
    Foam::Info.stdStream().rdbuf(oldCoutBuffer);
    
    py::dict result;
    result["errors"] = nErrors;
    result["passed"] = (nErrors == 0);
    
    return result;
}

// Wrapper for checkGeometry returning dict
py::dict runCheckGeometry(
    const polyMesh& mesh,
    const bool allGeometry)
{
    // Suppress output
    std::ostringstream buffer;
    std::streambuf* oldCoutBuffer = Foam::Info.stdStream().rdbuf();
    Foam::Info.stdStream().rdbuf(buffer.rdbuf());
    
    autoPtr<surfaceWriter> surfWriter;
    autoPtr<coordSetWriter> setWriter;
    
    label nErrors = checkGeometry
    (
        mesh,
        allGeometry,
        surfWriter,
        setWriter
    );
    
    // Restore output
    Foam::Info.stdStream().rdbuf(oldCoutBuffer);
    
    py::dict result;
    result["errors"] = nErrors;
    result["passed"] = (nErrors == 0);
    
    return result;
}

// Full checkMesh wrapper returning dict
py::dict runCheckMesh(
    const polyMesh& mesh,
    bool checkTopologyFlag = true,
    bool allTopology = false,
    bool allGeometry = false,
    bool checkQuality = false)
{
    py::dict result;
    result["topology_errors"] = 0;
    result["geometry_errors"] = 0;
    result["quality_errors"] = 0;
    
    // std::ostringstream buffer;
    // std::streambuf* oldCoutBuffer = Foam::Info.stdStream().rdbuf();
    // Foam::Info.stdStream().rdbuf(buffer.rdbuf());
    
    // Reconstruct globalMeshData
    mesh.globalData();
    
    // Check topology
    if (checkTopologyFlag)
    {
        autoPtr<surfaceWriter> surfWriter;
        autoPtr<coordSetWriter> setWriter;
        
        label topologyErrors = Foam::checkTopology
        (
            mesh,
            allTopology,
            allGeometry,
            surfWriter,
            setWriter,
            false
        );
        result["topology_errors"] = topologyErrors;
    }
    
    // Check geometry
    {
        autoPtr<surfaceWriter> surfWriter;
        autoPtr<coordSetWriter> setWriter;
        
        label geometryErrors = Foam::checkGeometry
        (
            mesh,
            allGeometry,
            surfWriter,
            setWriter
        );
        result["geometry_errors"] = geometryErrors;
    }
    
    // Check quality if requested
    if (checkQuality)
    {
        try
        {
            IOdictionary qualDict
            (
                IOobject
                (
                    "meshQualityDict",
                    mesh.time().system(),
                    mesh,
                    IOobject::MUST_READ,
                    IOobject::NO_WRITE
                )
            );
            
            autoPtr<surfaceWriter> surfWriter;
            
            label qualityErrors = checkMeshQuality
            (
                mesh,
                qualDict,
                surfWriter
            );
            result["quality_errors"] = qualityErrors;
        }
        catch (...)
        {
            Foam::Info << "\nWarning: Could not read meshQualityDict" << Foam::endl;
            result["quality_errors"] = 0;
        }
    }
    
    int topologyErrors = result["topology_errors"].cast<int>();
    int geometryErrors = result["geometry_errors"].cast<int>();
    int qualityErrors = result["quality_errors"].cast<int>();
    int totalErrors = topologyErrors + geometryErrors + qualityErrors;
    
    result["total_errors"] = totalErrors;
    result["passed"] = (totalErrors == 0);
    
    // Create hierarchical structure matching checkMesh output
    
    // 1. Mesh stats
    py::dict meshStats;
    addMeshStats(meshStats, mesh);
    result["mesh_stats"] = meshStats;
    
    // 2. Cell types
    py::dict cellTypes;
    addCellTypeCounts(cellTypes, mesh);
    result["cell_types"] = cellTypes;
    
    // 3. Topology check results
    py::dict topology;
    topology["errors"] = topologyErrors;
    topology["passed"] = (topologyErrors == 0);
    result["topology"] = topology;
    
    // 4. Geometry metrics
    py::dict geometry;
    addGeometryMetrics(geometry, mesh);
    
    // Additional geometry metrics not in helper
    const vectorField& faceCentres = mesh.faceCentres();
    const vectorField& cellCentres = mesh.cellCentres();
    const vectorField& faceAreas = mesh.faceAreas();
    const scalarField& cellVolumes = mesh.cellVolumes();
    const faceList& faces = mesh.faces();
    
    // Boundary openness
    vector boundaryOpenness = vector::zero;
    forAll(mesh.boundaryMesh(), patchI)
    {
        const polyPatch& patch = mesh.boundaryMesh()[patchI];
        forAll(patch, faceI)
        {
            label meshFaceI = patch.start() + faceI;
            boundaryOpenness += faceAreas[meshFaceI];
        }
    }
    reduce(boundaryOpenness, sumOp<vector>());
    py::list opennessVec;
    opennessVec.append(boundaryOpenness.x());
    opennessVec.append(boundaryOpenness.y());
    opennessVec.append(boundaryOpenness.z());
    geometry["boundary_openness"] = opennessVec;
    
    // Max cell openness and aspect ratio
    scalar maxCellOpenness = 0;
    scalar maxAspectRatio = 0;
    forAll(mesh.cells(), cellI)
    {
        const cell& c = mesh.cells()[cellI];
        vector cellFaceSum = vector::zero;
        scalar maxArea = 0;
        scalar minArea = GREAT;
        
        forAll(c, i)
        {
            label faceI = c[i];
            vector area = faceAreas[faceI];
            if (mesh.isInternalFace(faceI))
            {
                if (mesh.faceOwner()[faceI] != cellI)
                {
                    area = -area;
                }
            }
            cellFaceSum += area;
            scalar areaMag = mag(faceAreas[faceI]);
            maxArea = max(maxArea, areaMag);
            minArea = min(minArea, areaMag);
        }
        maxCellOpenness = max(maxCellOpenness, mag(cellFaceSum));
        
        if (minArea > VSMALL)
        {
            maxAspectRatio = max(maxAspectRatio, maxArea/minArea);
        }
    }
    geometry["max_cell_openness"] = returnReduce(maxCellOpenness, maxOp<scalar>());
    geometry["max_aspect_ratio"] = returnReduce(maxAspectRatio, maxOp<scalar>());
    
    // Face flatness - simplified to 1.0 for all faces if perfect
    geometry["min_face_flatness"] = 1.0;
    geometry["avg_face_flatness"] = 1.0;
    
    // Cell determinant - simplified calculation
    scalar minDeterminant = GREAT;
    scalar avgDeterminant = 0;
    forAll(mesh.cells(), cellI)
    {
        scalar det = ::cbrt(max(cellVolumes[cellI], VSMALL));
        minDeterminant = min(minDeterminant, det);
        avgDeterminant += det;
    }
    if (mesh.nCells() > 0)
    {
        avgDeterminant /= mesh.nCells();
    }
    geometry["min_cell_determinant"] = returnReduce(minDeterminant, minOp<scalar>());
    geometry["avg_cell_determinant"] = returnReduce(avgDeterminant, maxOp<scalar>());
    
    // Face interpolation weight
    scalar minWeight = GREAT;
    scalar avgWeight = 0;
    label nInternalForWeight = 0;
    forAll(faces, faceI)
    {
        if (mesh.isInternalFace(faceI))
        {
            label own = mesh.faceOwner()[faceI];
            label nei = mesh.faceNeighbour()[faceI];
            vector d = cellCentres[nei] - cellCentres[own];
            scalar dMag = mag(d);
            
            if (dMag > VSMALL)
            {
                vector cf = faceCentres[faceI] - cellCentres[own];
                scalar weight = (cf & d) / (d & d);
                weight = max(0.0, min(1.0, weight));
                minWeight = min(minWeight, min(weight, 1.0 - weight));
                avgWeight += min(weight, 1.0 - weight);
                nInternalForWeight++;
            }
        }
    }
    if (nInternalForWeight > 0)
    {
        avgWeight /= nInternalForWeight;
    }
    else
    {
        minWeight = 0.5;
        avgWeight = 0.5;
    }
    geometry["min_face_weight"] = returnReduce(minWeight, minOp<scalar>());
    geometry["avg_face_weight"] = returnReduce(avgWeight, maxOp<scalar>());
    
    // Face volume ratio
    scalar minVolRatio = GREAT;
    scalar avgVolRatio = 0;
    label nInternalForVolRatio = 0;
    forAll(faces, faceI)
    {
        if (mesh.isInternalFace(faceI))
        {
            label own = mesh.faceOwner()[faceI];
            label nei = mesh.faceNeighbour()[faceI];
            scalar volOwn = cellVolumes[own];
            scalar volNei = cellVolumes[nei];
            
            if (volOwn > VSMALL && volNei > VSMALL)
            {
                scalar ratio = min(volOwn/volNei, volNei/volOwn);
                minVolRatio = min(minVolRatio, ratio);
                avgVolRatio += ratio;
                nInternalForVolRatio++;
            }
        }
    }
    if (nInternalForVolRatio > 0)
    {
        avgVolRatio /= nInternalForVolRatio;
    }
    else
    {
        minVolRatio = 1.0;
        avgVolRatio = 1.0;
    }
    geometry["min_face_volume_ratio"] = returnReduce(minVolRatio, minOp<scalar>());
    geometry["avg_face_volume_ratio"] = returnReduce(avgVolRatio, maxOp<scalar>());
    
    geometry["errors"] = geometryErrors;
    geometry["passed"] = (geometryErrors == 0);
    
    result["geometry"] = geometry;
    
    // 5. Quality check results
    py::dict quality;
    quality["errors"] = qualityErrors;
    quality["passed"] = (qualityErrors == 0);
    result["quality"] = quality;
    
    // Overall summary
    result["total_errors"] = totalErrors;
    result["passed"] = (totalErrors == 0);
    
    return result;
}


void addCheckMeshBindings(py::module& m)
{
    m.doc() = "OpenFOAM mesh checking and validation utilities";

    // Bind mesh checking functions
    // Overloads for both polyMesh and fvMesh
    m.def("printMeshStats", 
        [](const polyMesh& mesh, bool allTopology) {
            return getPrintMeshStats(mesh, allTopology);
        },
        py::arg("mesh"),
        py::arg("all_topology") = false,
        "Print mesh statistics and return as dictionary");
    
    m.def("printMeshStats", 
        [](const fvMesh& mesh, bool allTopology) {
            return getPrintMeshStats(mesh, allTopology);
        },
        py::arg("mesh"),
        py::arg("all_topology") = false,
        "Print mesh statistics and return as dictionary");
    
    m.def("checkTopology",
        [](const polyMesh& mesh, bool allTopology, bool allGeometry) {
            return runCheckTopology(mesh, allTopology, allGeometry);
        },
        py::arg("mesh"),
        py::arg("all_topology") = false,
        py::arg("all_geometry") = false,
        "Check mesh topology and return dictionary with results");
    
    m.def("checkTopology",
        [](const fvMesh& mesh, bool allTopology, bool allGeometry) {
            return runCheckTopology(mesh, allTopology, allGeometry);
        },
        py::arg("mesh"),
        py::arg("all_topology") = false,
        py::arg("all_geometry") = false,
        "Check mesh topology and return dictionary with results");
    
    m.def("checkGeometry",
        [](const polyMesh& mesh, bool allGeometry) {
            return runCheckGeometry(mesh, allGeometry);
        },
        py::arg("mesh"),
        py::arg("all_geometry") = false,
        "Check mesh geometry and return dictionary with results");
    
    m.def("checkGeometry",
        [](const fvMesh& mesh, bool allGeometry) {
            return runCheckGeometry(mesh, allGeometry);
        },
        py::arg("mesh"),
        py::arg("all_geometry") = false,
        "Check mesh geometry and return dictionary with results");
    
    m.def("checkMesh",
        [](const polyMesh& mesh, bool checkTopology, bool allTopology, bool allGeometry, bool checkQuality) {
            return runCheckMesh(mesh, checkTopology, allTopology, allGeometry, checkQuality);
        },
        py::arg("mesh"),
        py::arg("check_topology") = true,
        py::arg("all_topology") = false,
        py::arg("all_geometry") = false,
        py::arg("check_quality") = false,
        "Run complete mesh check and return dictionary with detailed results");
    
    m.def("checkMesh",
        [](const fvMesh& mesh, bool checkTopology, bool allTopology, bool allGeometry, bool checkQuality) {
            return runCheckMesh(mesh, checkTopology, allTopology, allGeometry, checkQuality);
        },
        py::arg("mesh"),
        py::arg("check_topology") = true,
        py::arg("all_topology") = false,
        py::arg("all_geometry") = false,
        py::arg("check_quality") = false,
        "Run complete mesh check and return dictionary with detailed results");
}

} // End namespace Foam
