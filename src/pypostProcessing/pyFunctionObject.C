/*---------------------------------------------------------------------------*\
            Copyright (c) 2021, German Aerospace Center (DLR)
-------------------------------------------------------------------------------
License
    This file is part of the FMU4FOAM source code library, which is an
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

#include "pyFunctionObject.H"
#include "pyInterp.H"
#include "sigFpe.H"

namespace py = pybind11;
// using namespace py::literals;

namespace Foam
{
    defineTypeNameAndDebug(pyFunctionObject, 0);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //
Foam::pyFunctionObject::pyFunctionObject
(
    const fvMesh& mesh,
    word pymodule,
    word pyclass
)
: 
    mesh_(mesh),
    pyFuncObj_()
{
    pyInterp::New(mesh.time());

    // numpy causes a float point exception of loaded with OpenFOAM 
    // sigFpe so we temporally deactivate sigFpe we will only loose the 
    // stacktrace if deactivated
    sigFpe::unset(false);
    py::object pyC = py::module_::import(pymodule.c_str()).attr(pyclass.c_str());
    pyFuncObj_ = pyC(&mesh);
    sigFpe::set(false);
}
// ************************************************************************* //
