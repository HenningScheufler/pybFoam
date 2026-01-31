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

#include <pybind11/pybind11.h>

#include "bind_blockmesh.hpp"
#include "bind_checkmesh.hpp"
#include "bind_snappy.hpp"

namespace py = pybind11;


PYBIND11_MODULE(bind_checkmesh, m)
{
    m.doc() = R"pbdoc(
        OpenFOAM Meshing Python Bindings
        
        This module provides Python interfaces to OpenFOAM meshing
        utilities including blockMesh, checkMesh and snappyHexMesh.
    )pbdoc";

    // Add blockMesh bindings
    Foam::addBlockMeshBindings(m);

    // Add checkMesh bindings
    Foam::addCheckMeshBindings(m);

    // Add snappyHexMesh bindings
    Foam::addSnappyBindings(m);
}


// ************************************************************************* //
