/*---------------------------------------------------------------------------*\
            Copyright (c) 2021, German Aerospace Center (DLR)
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
#include "bind_io.hpp"
#include "bind_dict.hpp"
#include "bind_mesh.hpp"
#include "bind_primitives.hpp"
#include "bind_dimensioned.hpp"
#include "bind_fields.hpp"
#include "bind_geo_fields.hpp"
#include "bind_fvMatrix.hpp"
#include "bind_control.hpp"
#include "bind_cfdTools.hpp"

namespace py = pybind11;


PYBIND11_MODULE(pybFoam_core, m) {
    m.doc() = "python bindings for openfoam"; // optional module docstring

    Foam::bindIO(m);
    bindDict(m);
    bindMesh(m);
    bindPrimitives(m);
    bindDimensioned(m);
    Foam::bindFields(m);
    Foam::bindGeoFields(m);
    Foam::bindFvMatrix(m);
    Foam::bindControl(m);
    Foam::bindCfdTools(m);
}
