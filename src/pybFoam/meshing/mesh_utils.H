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

Class
    Foam::MeshUtils

Description
    Utility functions for mesh generation Python bindings.
    Provides conversion between Python objects and OpenFOAM data structures,
    and mesh statistics extraction.

\*---------------------------------------------------------------------------*/

#ifndef meshUtils_H
#define meshUtils_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "dictionary.H"
#include "polyMesh.H"
#include "Time.H"
#include "argList.H"

namespace py = pybind11;

namespace Foam
{

/*---------------------------------------------------------------------------*\
                        Class MeshUtils Declaration
\*---------------------------------------------------------------------------*/

class MeshUtils
{
public:

    // Static member functions

        //- Extract mesh statistics to Python dictionary
        static py::dict extractMeshStats(const polyMesh& mesh);

        //- Create and manage OpenFOAM Time object for mesh generation
        static autoPtr<Time> createTime
        (
            const fileName& casePath,
            const word& timeName = "constant"
        );

        //- Redirect OpenFOAM output streams
        static void redirectOutput(bool verbose);

        //- Restore OpenFOAM output streams
        static void restoreOutput();

private:

    // No private static data needed

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
