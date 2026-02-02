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

#include "bind_snappy.hpp"
#include "mesh_utils.H"

#include "argList.H"
#include "Time.H"
#include "fvMesh.H"
#include "IOdictionary.H"
#include "meshRefinement.H"
#include "snappyRefineDriver.H"
#include "snappySnapDriver.H"
#include "snappyLayerDriver.H"
#include "refinementSurfaces.H"
#include "refinementFeatures.H"
#include "shellSurfaces.H"
#include "decompositionMethod.H"
#include "fvMeshDistribute.H"
#include "refinementParameters.H"
#include "snapParameters.H"
#include "layerParameters.H"
#include "fvMeshTools.H"
#include "wallPolyPatch.H"
#include "coordSetWriter.H"
#include "surfaceWriter.H"
#include "searchableSurfaces.H"

#include <pybind11/stl.h>

namespace Foam
{

void generate_snappy_hex_mesh
(
    fvMesh& mesh,
    const dictionary& meshDict,
    bool overwrite,
    bool verbose
)
{
    MeshUtils::redirectOutput(verbose);

    const Time& runTime = mesh.time();
    const bool dryRun = false;

    // Check if we have the necessary subdicts
    if (!meshDict.found("geometry"))
    {
        FatalErrorInFunction << "geometry subdict not found in snappyHexMeshDict" << exit(FatalError);
    }

    // Writers for leak paths and closure surfaces
    autoPtr<coordSetWriter> setFormatter = coordSetWriter::New("vtk", dictionary::null);
    autoPtr<surfaceWriter> surfWriter = surfaceWriter::New("vtk", dictionary::null);
    refPtr<surfaceWriter> surfFormatter(std::move(surfWriter));

    // Geometry
    searchableSurfaces allGeometry
    (
        IOobject
        (
            "snappyHexMeshGeometry",
            runTime.constant(),
            "triSurface",
            runTime,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        ),
        meshDict.subDict("geometry"),
        true
    );

    const dictionary& refineDict = meshDict.subDict("castellatedMeshControls");
    const dictionary& snapDict = meshDict.subDict("snapControls");
    const dictionary& layerDict = meshDict.subDict("addLayersControls");

    // meshQualityControls is optional in some contexts but usually present
    const dictionary& qualityDict =
        meshDict.found("meshQualityControls")
        ? meshDict.subDict("meshQualityControls")
        : dictionary::null;

    // Surfaces
    refinementSurfaces surfaces
    (
        allGeometry,
        refineDict.subDict("refinementSurfaces"),
        refineDict.getOrDefault("gapLevelIncrement", 0),
        dryRun
    );

    // Features
    refinementFeatures features
    (
        mesh,
        PtrList<dictionary>
        (
            meshRefinement::lookup(refineDict, "features", dryRun)
        ),
        dryRun
    );

    // Shells
    shellSurfaces shells
    (
        allGeometry,
        meshRefinement::subDict(refineDict, "refinementRegions", dryRun),
        dryRun
    );

    // Limit shells
    shellSurfaces limitShells
    (
        allGeometry,
        refineDict.subOrEmptyDict("limitRegions"),
        dryRun
    );

    // Parameters
    refinementParameters refineParams(refineDict, dryRun);
    snapParameters snapParams(snapDict, dryRun);

    // IMPORTANT: Set refinement level of surface to be consistent with shells
    // This must be done BEFORE creating meshRefinement
    if (verbose)
    {
        Info << "Setting refinement level of surface to be consistent with shells." << endl;
    }
    surfaces.setMinLevelFields(shells);
    if (verbose)
    {
        Info << "Checked shell refinement" << endl;
    }

    // Calculate merge distance (following native snappyHexMesh)
    const scalar mergeTol = meshRefinement::get<scalar>
    (
        meshDict,
        "mergeTolerance",
        dryRun
    );
    const boundBox& meshBb = mesh.bounds();
    const scalar mergeDist = mergeTol * meshBb.mag();

    if (verbose)
    {
        Info << nl
            << "Overall mesh bounding box  : " << meshBb << nl
            << "Relative tolerance         : " << mergeTol << nl
            << "Absolute merge distance    : " << mergeDist << nl
            << endl;
    }

    // Set up mesh refiner
    meshRefinement meshRefiner
    (
        mesh,
        mergeDist,
        overwrite,
        surfaces,
        features,
        shells,
        limitShells,
        labelList(),
        dryRun
    );

    // CRITICAL: Calculate initial surface intersections
    // Without this, the meshRefiner doesn't know what faces to refine!
    if (verbose)
    {
        Info << "Determining initial surface intersections" << endl;
    }
    meshRefiner.updateIntersections(identity(mesh.nFaces()));
    if (verbose)
    {
        Info << "Calculated surface intersections" << endl;
    }

    // Add cellZones from surfaces
    const labelList namedSurfaces
    (
        surfaceZonesInfo::getNamedSurfaces(surfaces.surfZones())
    );
    labelList surfaceToCellZone = surfaceZonesInfo::addCellZonesToMesh
    (
        surfaces.surfZones(),
        namedSurfaces,
        mesh
    );

    // Add cellZones from refinement parameters
    refineParams.addCellZonesToMesh(mesh);

    // 5. Add patches to the mesh from surface regions
    labelList globalToMasterPatch(surfaces.nRegions(), -1);
    labelList globalToSlavePatch(surfaces.nRegions(), -1);

    const labelList& surfIndices = surfaces.surfaces();
    const PtrList<dictionary>& surfacePatchInfo = surfaces.patchInfo();

    forAll(surfIndices, surfi)
    {
        label geomi = surfIndices[surfi];
        const wordList& regNames = allGeometry.regionNames()[geomi];
        const wordList& fzNames = surfaces.surfZones()[surfi].faceZoneNames();

        if (fzNames.empty())
        {
            // 'Normal' surface
            forAll(regNames, regioni)
            {
                label globalRegioni = surfaces.globalRegion(surfi, regioni);
                label patchi = -1;

                if (surfacePatchInfo.set(globalRegioni))
                {
                    patchi = meshRefiner.addMeshedPatch(regNames[regioni], surfacePatchInfo[globalRegioni]);
                }
                else
                {
                    dictionary patchDict;
                    patchDict.set("type", "wall");
                    patchi = meshRefiner.addMeshedPatch(regNames[regioni], patchDict);
                }

                globalToMasterPatch[globalRegioni] = patchi;
                globalToSlavePatch[globalRegioni] = patchi;
            }
        }
        else
        {
            // Zoned surface
            forAll(regNames, regioni)
            {
                label globalRegioni = surfaces.globalRegion(surfi, regioni);

                // Add master side patch
                label patchi = -1;
                if (surfacePatchInfo.set(globalRegioni))
                {
                    patchi = meshRefiner.addMeshedPatch(regNames[regioni], surfacePatchInfo[globalRegioni]);
                }
                else
                {
                    dictionary patchDict;
                    patchDict.set("type", "wall");
                    patchi = meshRefiner.addMeshedPatch(regNames[regioni], patchDict);
                }
                globalToMasterPatch[globalRegioni] = patchi;

                // Add slave side patch
                const word slaveName = regNames[regioni] + "_slave";
                label slavePatchi = -1;
                if (surfacePatchInfo.set(globalRegioni))
                {
                    slavePatchi = meshRefiner.addMeshedPatch(slaveName, surfacePatchInfo[globalRegioni]);
                }
                else
                {
                    dictionary patchDict;
                    patchDict.set("type", "wall");
                    slavePatchi = meshRefiner.addMeshedPatch(slaveName, patchDict);
                }
                globalToSlavePatch[globalRegioni] = slavePatchi;
            }
        }
    }

    // Re-do intersections on meshed boundaries since they use an extrapolated other side
    {
        const labelList adaptPatchIDs(meshRefiner.meshedPatches());
        const polyBoundaryMesh& pbm = mesh.boundaryMesh();

        label nFaces = 0;
        forAll(adaptPatchIDs, i)
        {
            nFaces += pbm[adaptPatchIDs[i]].size();
        }

        labelList faceLabels(nFaces);
        nFaces = 0;
        forAll(adaptPatchIDs, i)
        {
            const polyPatch& pp = pbm[adaptPatchIDs[i]];
            forAll(pp, i)
            {
                faceLabels[nFaces++] = pp.start() + i;
            }
        }
        meshRefiner.updateIntersections(faceLabels);
    }

    // Decomposition and Distribute
    // For now, simpler version. In production we might want to load decomposeParDict
    dictionary decomposeDict;
    decomposeDict.set("method", "hierarchical");
    decomposeDict.set("numberOfSubdomains", 1);
    dictionary hierarchicalCoeffs;
    hierarchicalCoeffs.set("n", "(1 1 1)");
    decomposeDict.set("hierarchicalCoeffs", hierarchicalCoeffs);

    autoPtr<decompositionMethod> decomposerPtr(decompositionMethod::New(decomposeDict));
    fvMeshDistribute distributor(mesh);

    // Logic for co-planar faces
    meshRefinement::FaceMergeType mergeType = meshRefinement::FaceMergeType::GEOMETRIC;

    // Phases
    bool wantRefine = meshDict.getOrDefault("castellatedMesh", true);
    bool wantSnap = meshDict.getOrDefault("snap", true);
    bool wantLayers = meshDict.getOrDefault("addLayers", false);

    // IMPORTANT: Set refinement level of surface to be consistent with curvature
    // This must happen after patches are added but before refinement starts
    if (verbose)
    {
        Info << "Setting refinement level of surface to be consistent with curvature." << endl;
    }
    surfaces.setCurvatureMinLevelFields
    (
        refineParams.curvature(),
        refineParams.planarAngle()
    );
    if (verbose)
    {
        Info << "Checked curvature refinement" << endl;
    }

    if (wantRefine)
    {
        snappyRefineDriver refineDriver
        (
            meshRefiner,
            *decomposerPtr,
            distributor,
            globalToMasterPatch,
            globalToSlavePatch,
            *setFormatter,
            surfFormatter,
            false  // dryRun
        );

        refineDriver.doRefine
        (
            refineDict,
            refineParams,
            snapParams,
            refineParams.handleSnapProblems(),
            mergeType,
            qualityDict
        );
    }

    if (wantSnap)
    {
        snappySnapDriver snapDriver
        (
            meshRefiner,
            globalToMasterPatch,
            globalToSlavePatch,
            false // dryRun
        );

        snapDriver.doSnap
        (
            snapDict,
            qualityDict,
            mergeType,
            refineParams.curvature(),
            refineParams.planarAngle(),
            snapParams
        );
    }

    if (wantLayers)
    {
        layerParameters layerParams(layerDict, mesh.boundaryMesh(), false);

        snappyLayerDriver layerDriver
        (
            meshRefiner,
            globalToMasterPatch,
            globalToSlavePatch,
            false // dryRun
        );

        layerDriver.doLayers
        (
            layerDict,
            qualityDict,
            layerParams,
            mergeType,
            false, // preBalance
            *decomposerPtr,
            distributor
        );
    }

    // Cleanup
    fvMeshTools::removeEmptyPatches(mesh, true);

    // Write the mesh
    if (verbose)
    {
        Info << "Writing mesh to time " << meshRefiner.timeName() << endl;
    }

    meshRefiner.write
    (
        meshRefinement::debugType(0),
        meshRefinement::writeType(meshRefinement::WRITEMESH),
        mesh.time().path()/meshRefiner.timeName()
    );

    if (verbose)
    {
        Info << "snappyHexMesh completed" << endl;
    }

    MeshUtils::restoreOutput();
}

void addSnappyBindings(pybind11::module_& m)
{
    m.def("generate_snappy_hex_mesh",
        [](fvMesh& mesh, const dictionary& dict, bool overwrite, bool verbose)
        {
            generate_snappy_hex_mesh(mesh, dict, overwrite, verbose);
        },
        pybind11::arg("mesh"),
        pybind11::arg("dict"),
        pybind11::arg("overwrite") = true,
        pybind11::arg("verbose") = true,
        "Run snappyHexMesh on an existing mesh"
    );
}

} // End namespace Foam
