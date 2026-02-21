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

nb::dict Foam::MeshUtils::extractMeshStats(const polyMesh& mesh)
{
    namespace nb = nanobind;
    nb::dict stats;

    // Basic mesh statistics
    stats["nPoints"] = nb::cast(mesh.nPoints());
    stats["nCells"] = nb::cast(mesh.nCells());
    stats["nFaces"] = nb::cast(mesh.nFaces());
    stats["nInternalFaces"] = nb::cast(mesh.nInternalFaces());
    stats["nBoundaryFaces"] = nb::cast(mesh.nBoundaryFaces());

    // Boundary patch information
    nb::list patches;
    const polyBoundaryMesh& boundaryMesh = mesh.boundaryMesh();

    forAll(boundaryMesh, patchi)
    {
        const polyPatch& patch = boundaryMesh[patchi];
        nb::dict patchInfo;
        patchInfo["name"] = nb::cast(std::string(patch.name()));
        patchInfo["type"] = nb::cast(std::string(patch.type()));
        patchInfo["nFaces"] = nb::cast(patch.size());
        patchInfo["startFace"] = nb::cast(patch.start());
        patches.append(patchInfo);
    }

    stats["patches"] = patches;

    // Mesh bounds
    const boundBox& bb = mesh.bounds();
    nb::list minBounds;
    minBounds.append(nb::cast(bb.min().x()));
    minBounds.append(nb::cast(bb.min().y()));
    minBounds.append(nb::cast(bb.min().z()));

    nb::list maxBounds;
    maxBounds.append(nb::cast(bb.max().x()));
    maxBounds.append(nb::cast(bb.max().y()));
    maxBounds.append(nb::cast(bb.max().z()));

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
