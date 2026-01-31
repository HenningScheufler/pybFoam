/*---------------------------------------------------------------------------*\
            Copyright (c) 2022, Henning Scheufler
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

Class
    Foam::polyMesh bindings

Description
    Provides Python bindings for OpenFOAM polyMesh class and related
    functionality including mesh topology, boundary patches, and mesh construction.

Author
    Henning Scheufler, all rights reserved.

\*---------------------------------------------------------------------------*/

#ifndef foam_polymesh
#define foam_polymesh

// System includes
#include <pybind11/pybind11.h>
#include "polyMesh.H"
#include "IOobject.H"
#include "pointField.H"
#include "faceList.H"
#include "labelList.H"
#include <pybind11/stl.h>
#include <vector>
#include <tuple>
#include <string>

namespace Foam
{
    // Helper functions for polyMesh creation
    
    polyMesh* createPolyMesh(
        const IOobject& io,
        const pointField& points,
        const faceList& faces,
        const labelList& owner,
        const labelList& neighbour,
        bool syncPar = true);

    polyMesh* createPolyMeshFromPython(
        const IOobject& io,
        const std::vector<std::vector<double>>& points,
        const std::vector<std::vector<Foam::label>>& faces,
        const std::vector<Foam::label>& owner,
        const std::vector<Foam::label>& neighbour,
        bool syncPar = true);

    polyMesh* createPolyMeshFromCellShapes(
        const IOobject& io,
        const std::vector<std::vector<double>>& points,
        const std::vector<std::tuple<std::string, std::vector<Foam::label>>>& cells,
        const std::vector<std::tuple<std::string, std::vector<std::vector<Foam::label>>>>& boundaryPatches,
        const std::string& defaultPatchName = "defaultFaces",
        bool syncPar = true);

    // Main binding function
    void bindPolyMesh(pybind11::module_& m);
}

#endif
