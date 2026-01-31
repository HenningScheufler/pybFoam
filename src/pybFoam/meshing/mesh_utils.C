/*---------------------------------------------------------------------------*\
            Copyright (c) 2021-2026, German Aerospace Center (DLR)
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

#include "mesh_utils.H"
#include "IStringStream.H"
#include "OStringStream.H"

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

py::dict Foam::MeshUtils::extractMeshStats(const polyMesh& mesh)
{
    py::dict stats;
    
    // Basic mesh statistics
    stats["nPoints"] = mesh.nPoints();
    stats["nCells"] = mesh.nCells();
    stats["nFaces"] = mesh.nFaces();
    stats["nInternalFaces"] = mesh.nInternalFaces();
    stats["nBoundaryFaces"] = mesh.nBoundaryFaces();
    
    // Boundary patch information
    py::list patches;
    const polyBoundaryMesh& boundaryMesh = mesh.boundaryMesh();
    
    forAll(boundaryMesh, patchi)
    {
        const polyPatch& patch = boundaryMesh[patchi];
        py::dict patchInfo;
        patchInfo["name"] = py::str(patch.name());
        patchInfo["type"] = py::str(patch.type());
        patchInfo["nFaces"] = patch.size();
        patchInfo["startFace"] = patch.start();
        patches.append(patchInfo);
    }
    
    stats["patches"] = patches;
    
    // Mesh bounds
    const boundBox& bb = mesh.bounds();
    py::list minBounds = py::list();
    minBounds.append(bb.min().x());
    minBounds.append(bb.min().y());
    minBounds.append(bb.min().z());
    
    py::list maxBounds = py::list();
    maxBounds.append(bb.max().x());
    maxBounds.append(bb.max().y());
    maxBounds.append(bb.max().z());
    
    stats["bounds_min"] = minBounds;
    stats["bounds_max"] = maxBounds;
    
    return stats;
}


Foam::autoPtr<Foam::Time> Foam::MeshUtils::createTime
(
    const fileName& casePath,
    const word& timeName
)
{
    // Create Time object directly without argList
    // OpenFOAM Time can be instantiated standalone for library use
    autoPtr<Time> runTimePtr;
    runTimePtr.reset
    (
        new Time
        (
            Time::controlDictName,
            casePath
        )
    );
    
    // Set time using instant (timeName as word, 0 as index)
    runTimePtr->setTime(Foam::instant(timeName), 0);
    
    return runTimePtr;
}


void Foam::MeshUtils::redirectOutput(bool verbose)
{
    // OpenFOAM messageStream doesn't support direct stream redirection
    // Verbosity is controlled by the calling code
    // This is a placeholder for potential future implementation
}


void Foam::MeshUtils::restoreOutput()
{
    // Placeholder - output is controlled by OpenFOAM's own mechanisms
}


// ************************************************************************* //
