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
    Foam::fvMesh bindings

Description
    Provides Python bindings for OpenFOAM fvMesh class and related
    functionality including finite volume mesh operations and boundary conditions.

Author
    Henning Scheufler, all rights reserved.

\*---------------------------------------------------------------------------*/

#ifndef foam_bind_fvmesh
#define foam_bind_fvmesh

// System includes
#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include "fvMesh.H"
#include "Time.H"
#include "polyMesh.H"


namespace Foam
{
    fvMesh* createMesh(const Time& time, bool autoWrite = false);

    fvMesh* createMeshFromPolyMesh(polyMesh& polyMeshRef, bool autoWrite = false);
}


void bindFvMesh(nanobind::module_& m);


#endif // foam_dict  defined
