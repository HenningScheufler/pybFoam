/*---------------------------------------------------------------------------*\
            Copyright (c) 20212, Henning Scheufler
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

#include "bind_thermo.hpp"
#include "instantList.H"
#include "fluidThermo.H"
#include "solidThermo.H"
#include "basicThermo.H"
#include "compressibleTransportModel.H"


namespace py = pybind11;

template <typename... Args>
using overload_cast_ = py::detail::overload_cast_impl<Args...>;

void Foam::bindThermo(py::module& m)
{


    py::class_<basicThermo>(m, "basicThermo")
    .def_static("from_registry",[](const fvMesh& mesh)
    {
        const basicThermo* obj = mesh.findObject<basicThermo>(basicThermo::dictName);
        return obj;
    },py::return_value_policy::reference)
    .def("p", overload_cast_< >()(&Foam::basicThermo::p, py::const_))
    .def("T", overload_cast_< >()(&Foam::basicThermo::T, py::const_))
    .def("rho", overload_cast_< >()(&Foam::basicThermo::rho, py::const_))
    .def("he", overload_cast_< >()(&Foam::basicThermo::he, py::const_))
    .def("Cp", overload_cast_< >()(&Foam::basicThermo::Cp, py::const_))
    .def("Cv", overload_cast_< >()(&Foam::basicThermo::Cv, py::const_))
    .def("kappa", overload_cast_< >()(&Foam::basicThermo::kappa, py::const_))
    .def("kappaEff", overload_cast_<const volScalarField& >()(&Foam::basicThermo::kappaEff, py::const_))
    .def("alphaEff", overload_cast_<const volScalarField& >()(&Foam::basicThermo::alphaEff, py::const_))
    ;

    // auto sf = declare_thermo<fluidThermo>(m, std::string("fluidThermo"));

    py::class_<fluidThermo,basicThermo>(m, "fluidThermo")
    .def_static("from_registry",[](const fvMesh& mesh)
    {
        const fluidThermo* obj = mesh.findObject<fluidThermo>(fluidThermo::dictName);
        return obj;
    },py::return_value_policy::reference)
    ;

    py::class_<solidThermo,basicThermo>(m, "solidThermo")
    .def_static("from_registry",[](const fvMesh& mesh)
    {
        const solidThermo* obj = mesh.findObject<solidThermo>(solidThermo::dictName);
        return obj;
    },py::return_value_policy::reference)
    ;

}
