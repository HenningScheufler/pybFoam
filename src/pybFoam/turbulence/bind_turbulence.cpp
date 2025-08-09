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

namespace py = pybind11;

template <typename... Args>
using overload_cast_ = py::detail::overload_cast_impl<Args...>;

void Foam::bindTurbulence(py::module& m)
{

    py::class_<singlePhaseTransportModel>(m, "singlePhaseTransportModel")
        .def(py::init<const volVectorField&, const surfaceScalarField&>(),
             py::arg("U"), py::arg("phi"))
        .def("correct", &singlePhaseTransportModel::correct)
    ;

    py::class_<incompressible::turbulenceModel>(m, "incompressibleTurbulenceModel")
    .def_static("from_registry",[](const fvMesh& mesh)
    {
        const incompressible::turbulenceModel* obj = mesh.findObject<incompressible::turbulenceModel>(incompressible::turbulenceModel::propertiesName);
        return obj;
    }, py::return_value_policy::reference) 
    // from turbulenceModel
    .def_static("New",[](const volVectorField& U,
        const surfaceScalarField& phi,
        const singlePhaseTransportModel& transportModel,
        const word& propertiesName)
    {
        return incompressible::turbulenceModel::New(U, phi, transportModel, propertiesName).ptr();
    }, py::arg("U"), py::arg("phi"), py::arg("transportModel"), py::arg("propertiesName") = turbulenceModel::propertiesName, py::return_value_policy::take_ownership)
    .def("correct", &incompressible::turbulenceModel::correct)
    .def("U", &incompressible::turbulenceModel::U)
    .def("alphaRhoPhi", &incompressible::turbulenceModel::alphaRhoPhi)
    .def("phi", &incompressible::turbulenceModel::phi)
    .def("y",&incompressible::turbulenceModel::y)
    .def("k", &incompressible::turbulenceModel::k)
    .def("epsilon", &incompressible::turbulenceModel::epsilon)
    .def("omega", &incompressible::turbulenceModel::omega)
    .def("R", &incompressible::turbulenceModel::R)
    .def("nu", overload_cast_< >()(&incompressible::turbulenceModel::nu, py::const_))
    .def("nu", overload_cast_<const label >()(&incompressible::turbulenceModel::nu, py::const_))
    .def("nut", overload_cast_< >()(&incompressible::turbulenceModel::nut, py::const_))
    .def("nut", overload_cast_<const label >()(&incompressible::turbulenceModel::nut, py::const_))
    .def("mu", overload_cast_< >()(&incompressible::turbulenceModel::mu, py::const_))
    .def("mu", overload_cast_<const label >()(&incompressible::turbulenceModel::mu, py::const_))
    .def("mut", overload_cast_< >()(&incompressible::turbulenceModel::mut, py::const_))
    .def("mut", overload_cast_<const label >()(&incompressible::turbulenceModel::mut, py::const_))
    .def("muEff", overload_cast_< >()(&incompressible::turbulenceModel::muEff, py::const_))
    .def("muEff", overload_cast_<const label >()(&incompressible::turbulenceModel::muEff, py::const_))
    .def("nuEff", overload_cast_< >()(&incompressible::turbulenceModel::nuEff, py::const_))
    .def("nuEff", overload_cast_<const label >()(&incompressible::turbulenceModel::nuEff, py::const_))
    .def("devRhoReff", overload_cast_< >()(&incompressible::turbulenceModel::devRhoReff, py::const_))
    .def("divDevReff", overload_cast_<volVectorField&>()(&incompressible::turbulenceModel::divDevReff, py::const_))
    #if OPENFOAM > 2106
        .def("devRhoReff", overload_cast_<const volVectorField&>()(&incompressible::turbulenceModel::devRhoReff, py::const_))
    #endif
    ;

    
    py::class_<compressible::turbulenceModel>(m, "compressibleTurbulenceModel",py::module_local())
    .def_static("from_registry",[](const fvMesh& mesh)
    {
        const compressible::turbulenceModel* obj = mesh.findObject<compressible::turbulenceModel>(compressible::turbulenceModel::propertiesName);
        return obj;
    },py::return_value_policy::reference) 
    // from turbulenceModel
    .def("U", &compressible::turbulenceModel::U)
    .def("alphaRhoPhi", &compressible::turbulenceModel::alphaRhoPhi)
    .def("phi", &compressible::turbulenceModel::phi)
    .def("y",&compressible::turbulenceModel::y)
    .def("k", &compressible::turbulenceModel::k)
    .def("epsilon", &compressible::turbulenceModel::epsilon)
    .def("omega", &compressible::turbulenceModel::omega)
    .def("R", &compressible::turbulenceModel::R)
    .def("nu", overload_cast_< >()(&compressible::turbulenceModel::nu, py::const_))
    .def("nu", overload_cast_<const label >()(&compressible::turbulenceModel::nu, py::const_))
    .def("nut", overload_cast_< >()(&compressible::turbulenceModel::nut, py::const_))
    .def("nut", overload_cast_<const label >()(&compressible::turbulenceModel::nut, py::const_))
    .def("mu", overload_cast_< >()(&compressible::turbulenceModel::mu, py::const_))
    .def("mu", overload_cast_<const label >()(&compressible::turbulenceModel::mu, py::const_))
    .def("mut", overload_cast_< >()(&compressible::turbulenceModel::mut, py::const_))
    .def("mut", overload_cast_<const label >()(&compressible::turbulenceModel::mut, py::const_))
    .def("muEff", overload_cast_< >()(&compressible::turbulenceModel::muEff, py::const_))
    .def("muEff", overload_cast_<const label >()(&compressible::turbulenceModel::muEff, py::const_))
    .def("nuEff", overload_cast_< >()(&compressible::turbulenceModel::nuEff, py::const_))
    .def("nuEff", overload_cast_<const label >()(&compressible::turbulenceModel::nuEff, py::const_))
    .def("devRhoReff", overload_cast_< >()(&compressible::turbulenceModel::devRhoReff, py::const_))
    .def("divDevRhoReff", overload_cast_<volVectorField&>()(&compressible::turbulenceModel::divDevRhoReff, py::const_))
    #if OPENFOAM > 2106
        .def("devRhoReff", overload_cast_<const volVectorField&>()(&compressible::turbulenceModel::devRhoReff, py::const_))
    #endif
    ;

}
