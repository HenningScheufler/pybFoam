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

Description
    Simplified Python bindings for OpenFOAM blockMesh utility.

\*---------------------------------------------------------------------------*/

#ifndef bindBlockmesh_H
#define bindBlockmesh_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "dictionary.H"
#include "polyMesh.H"
#include "Time.H"

namespace py = pybind11;

namespace Foam
{

/*---------------------------------------------------------------------------*\
                    Function declarations
\*---------------------------------------------------------------------------*/

//- Generate blockMesh from OpenFOAM dictionary and return polyMesh
polyMesh* generateBlockMesh
(
    Time& runTime,
    const dictionary& blockMeshDict,
    bool verbose = false,
    const std::string& timeName = "constant"
);

//- Add Python bindings for blockMesh functions
void addBlockMeshBindings(py::module_& m);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
