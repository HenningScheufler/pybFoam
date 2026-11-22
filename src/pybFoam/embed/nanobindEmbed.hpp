/*---------------------------------------------------------------------------*\
            Copyright (c) 2026, Henning Scheufler
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

Function
    Foam::initEmbeddedNanobind

Description
    Connect a shared library without NB_MODULE to pybFoam's nanobind domain.

    nanobind 3 keeps its state pointer in a hidden variable, so every shared
    library has its own copy and only NB_MODULE or nb::register_module() sets
    it. A library that embeds Python has no NB_MODULE and must call this
    function, after pyInterp::New() and before any other nanobind call.

    The function has internal linkage on purpose. As an ordinary inline
    function it would be exported, and the dynamic linker would bind every
    library to a single definition, which sets only that library's pointer.

    Registering pybFoam_core itself selects its domain, so types such as
    fvMesh cast, and ties the reference to a module that lives until the
    interpreter shuts down. Repeated calls are harmless.

    Importing pybFoam loads numpy, so floating point traps must be disabled
    around the call.

\*---------------------------------------------------------------------------*/

#ifndef pybFoamNanobindEmbed_H
#define pybFoamNanobindEmbed_H

#include <nanobind/nanobind.h>

#include "error.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

static inline void initEmbeddedNanobind()
{
    PyObject* core = PyImport_ImportModule("pybFoam.pybFoam_core");

    if (!core || !nanobind::register_module(core))
    {
        PyErr_Print();
        Py_XDECREF(core);

        FatalErrorInFunction
            << "Could not register with the nanobind domain of pybFoam"
            << exit(FatalError);
    }

    Py_DECREF(core);
}

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
