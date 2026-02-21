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

#include "bind_turbulence.hpp"
#include "instantList.H"

#include "turbulentTransportModel.H"
#include "singlePhaseTransportModel.H"

#include "compressibleTransportModel.H"
#include "turbulentFluidThermoModel.H"
#include "fluidThermo.H"

#include "volFields.H"
#include "GeometricFields.H"

namespace nb = nanobind;

void Foam::bindTurbulence(nb::module_& m)
{

    nb::class_<singlePhaseTransportModel>(m, "singlePhaseTransportModel")
        .def(nb::init<const volVectorField&, const surfaceScalarField&>(),
             nb::arg("U"), nb::arg("phi"))
        .def("correct", &singlePhaseTransportModel::correct)
    ;

    nb::class_<incompressible::turbulenceModel>(m, "incompressibleTurbulenceModel")
    .def_static("from_registry",[](const fvMesh& mesh)
    {
        const incompressible::turbulenceModel* obj = mesh.findObject<incompressible::turbulenceModel>(incompressible::turbulenceModel::propertiesName);
        return obj;
    }, nb::rv_policy::reference)
    // from turbulenceModel
    .def_static("New",[](const volVectorField& U,
        const surfaceScalarField& phi,
        const singlePhaseTransportModel& transportModel,
        const word& propertiesName)
    {
        return incompressible::turbulenceModel::New(U, phi, transportModel, propertiesName).ptr();
    }, nb::arg("U"), nb::arg("phi"), nb::arg("transportModel"), nb::arg("propertiesName") = turbulenceModel::propertiesName, nb::rv_policy::take_ownership)
    .def("correct", &incompressible::turbulenceModel::correct)
    .def("U", &incompressible::turbulenceModel::U)
    .def("alphaRhoPhi", &incompressible::turbulenceModel::alphaRhoPhi)
    .def("phi", &incompressible::turbulenceModel::phi)
    // .def("y",&incompressible::turbulenceModel::y) // nearWallDist not yet bound
    .def("k", &incompressible::turbulenceModel::k)
    .def("epsilon", &incompressible::turbulenceModel::epsilon)
    .def("omega", &incompressible::turbulenceModel::omega)
    .def("R", &incompressible::turbulenceModel::R)
    .def("nu", [](const incompressible::turbulenceModel& self) { return self.nu(); })
    .def("nu", [](const incompressible::turbulenceModel& self, const label i) { return self.nu(i); })
    .def("nut", [](const incompressible::turbulenceModel& self) { return self.nut(); })
    .def("nut", [](const incompressible::turbulenceModel& self, const label i) { return self.nut(i); })
    .def("mu", [](const incompressible::turbulenceModel& self) { return self.mu(); })
    .def("mu", [](const incompressible::turbulenceModel& self, const label i) { return self.mu(i); })
    .def("mut", [](const incompressible::turbulenceModel& self) { return self.mut(); })
    .def("mut", [](const incompressible::turbulenceModel& self, const label i) { return self.mut(i); })
    .def("muEff", [](const incompressible::turbulenceModel& self) { return self.muEff(); })
    .def("muEff", [](const incompressible::turbulenceModel& self, const label i) { return self.muEff(i); })
    .def("nuEff", [](const incompressible::turbulenceModel& self) { return self.nuEff(); })
    .def("nuEff", [](const incompressible::turbulenceModel& self, const label i) { return self.nuEff(i); })
    .def("devRhoReff", [](const incompressible::turbulenceModel& self) { return self.devRhoReff(); })
    .def("divDevReff", [](const incompressible::turbulenceModel& self, volVectorField& U) { return self.divDevReff(U); })
    #if OPENFOAM > 2106
        .def("devRhoReff", [](const incompressible::turbulenceModel& self, const volVectorField& U) { return self.devRhoReff(U); })
    #endif
    ;


    nb::class_<compressible::turbulenceModel>(m, "compressibleTurbulenceModel")
    .def_static("from_registry",[](const fvMesh& mesh)
    {
        const compressible::turbulenceModel* obj = mesh.findObject<compressible::turbulenceModel>(compressible::turbulenceModel::propertiesName);
        return obj;
    },nb::rv_policy::reference)
    // from turbulenceModel
    .def("U", &compressible::turbulenceModel::U)
    .def("alphaRhoPhi", &compressible::turbulenceModel::alphaRhoPhi)
    .def("phi", &compressible::turbulenceModel::phi)
    // .def("y",&compressible::turbulenceModel::y) // nearWallDist not yet bound
    .def("k", &compressible::turbulenceModel::k)
    .def("epsilon", &compressible::turbulenceModel::epsilon)
    .def("omega", &compressible::turbulenceModel::omega)
    .def("R", &compressible::turbulenceModel::R)
    .def("nu", [](const compressible::turbulenceModel& self) { return self.nu(); })
    .def("nu", [](const compressible::turbulenceModel& self, const label i) { return self.nu(i); })
    .def("nut", [](const compressible::turbulenceModel& self) { return self.nut(); })
    .def("nut", [](const compressible::turbulenceModel& self, const label i) { return self.nut(i); })
    .def("mu", [](const compressible::turbulenceModel& self) { return self.mu(); })
    .def("mu", [](const compressible::turbulenceModel& self, const label i) { return self.mu(i); })
    .def("mut", [](const compressible::turbulenceModel& self) { return self.mut(); })
    .def("mut", [](const compressible::turbulenceModel& self, const label i) { return self.mut(i); })
    .def("muEff", [](const compressible::turbulenceModel& self) { return self.muEff(); })
    .def("muEff", [](const compressible::turbulenceModel& self, const label i) { return self.muEff(i); })
    .def("nuEff", [](const compressible::turbulenceModel& self) { return self.nuEff(); })
    .def("nuEff", [](const compressible::turbulenceModel& self, const label i) { return self.nuEff(i); })
    .def("devRhoReff", [](const compressible::turbulenceModel& self) { return self.devRhoReff(); })
    .def("divDevRhoReff", [](const compressible::turbulenceModel& self, volVectorField& U) { return self.divDevRhoReff(U); })
    #if OPENFOAM > 2106
        .def("devRhoReff", [](const compressible::turbulenceModel& self, const volVectorField& U) { return self.devRhoReff(U); })
    #endif
    ;

}
