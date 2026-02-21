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


namespace nb = nanobind;

void Foam::bindThermo(nb::module_& m)
{


    nb::class_<basicThermo>(m, "basicThermo")
    .def_static("from_registry",[](const fvMesh& mesh)
    {
        const basicThermo* obj = mesh.findObject<basicThermo>(basicThermo::dictName);
        return obj;
    },nb::rv_policy::reference)
    .def("p", [](const Foam::basicThermo& self) { return self.p(); })
    .def("T", [](const Foam::basicThermo& self) { return self.T(); })
    .def("rho", [](const Foam::basicThermo& self) { return self.rho(); })
    .def("he", [](const Foam::basicThermo& self) { return self.he(); })
    .def("Cp", [](const Foam::basicThermo& self) { return self.Cp(); })
    .def("Cv", [](const Foam::basicThermo& self) { return self.Cv(); })
    .def("kappa", [](const Foam::basicThermo& self) { return self.kappa(); })
    .def("kappaEff", [](const Foam::basicThermo& self, const volScalarField& alphat) { return self.kappaEff(alphat); })
    .def("alphaEff", [](const Foam::basicThermo& self, const volScalarField& alphat) { return self.alphaEff(alphat); })
    ;

    // auto sf = declare_thermo<fluidThermo>(m, std::string("fluidThermo"));

    nb::class_<fluidThermo,basicThermo>(m, "fluidThermo")
    .def_static("from_registry",[](const fvMesh& mesh)
    {
        const fluidThermo* obj = mesh.findObject<fluidThermo>(fluidThermo::dictName);
        return obj;
    },nb::rv_policy::reference)
    ;

    nb::class_<solidThermo,basicThermo>(m, "solidThermo")
    .def_static("from_registry",[](const fvMesh& mesh)
    {
        const solidThermo* obj = mesh.findObject<solidThermo>(solidThermo::dictName);
        return obj;
    },nb::rv_policy::reference)
    ;

}
