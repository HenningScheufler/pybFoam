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

#include "bind_dimensioned.hpp"
#include "dimensionedType.H"
#include "dimensionSet.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "tmp.H"

namespace Foam
{
    // Type aliases to help pybind11-stubgen parse complex template types
    using volScalarField = GeometricField<scalar, fvPatchField, volMesh>;
    using volVectorField = GeometricField<vector, fvPatchField, volMesh>;
    using volTensorField = GeometricField<tensor, fvPatchField, volMesh>;
    using surfaceScalarField = GeometricField<scalar, fvsPatchField, surfaceMesh>;
    using surfaceVectorField = GeometricField<vector, fvsPatchField, surfaceMesh>;

    template<class Type>
    auto declare_dimensioned(py::module &m, std::string className)
    {
        return py::class_<dimensioned<Type>>(m, className.c_str())
            .def(py::init<const word&,  const dimensionSet, const Type&>(),
                 py::arg("name"), py::arg("dimensions"), py::arg("value"))
            .def(py::init<const word&, const dimensionSet, const dictionary&>(),
                 py::arg("name"), py::arg("dimensions"), py::arg("dict"))
            .def(py::init([](const std::string& name, const dimensionSet& dims, const Type& value) {
                return dimensioned<Type>(word(name), dims, value);
            }), py::arg("name"), py::arg("dimensions"), py::arg("value"))
            .def("name", [](const dimensioned<Type>& self) { return std::string(self.name()); })
            .def("dimensions", [](const dimensioned<Type>& self) { return self.dimensions(); })
            .def("value", [](const dimensioned<Type>& self) { return self.value(); })

            .def("__mul__", [](const dimensioned<Type>& self, const volScalarField& field) {
                return self * field;
            }, py::arg("field"), "Multiply dimensioned value by volScalarField")
            .def("__mul__", [](const dimensioned<Type>& self, const tmp<volScalarField>& field) {
                return self * field;
            }, py::arg("field"), "Multiply dimensioned value by tmp<volScalarField>")

            .def("__mul__", [](const dimensioned<Type>& self, const surfaceScalarField& field) {
                return self * field;
            }, py::arg("field"), "Multiply dimensioned value by surfaceScalarField")
            .def("__mul__", [](const dimensioned<Type>& self, const tmp<surfaceScalarField>& field) {
                return self * field;
            }, py::arg("field"), "Multiply dimensioned value by tmp<surfaceScalarField>")

            .def("__add__", [](const dimensioned<Type>& self, const GeometricField<Type, fvPatchField, volMesh>& field) {
                return self + field;
            }, py::arg("field"), "Add dimensioned value to volField")
            .def("__add__", [](const dimensioned<Type>& self, const tmp<GeometricField<Type, fvPatchField, volMesh>>& field) {
                return self + field;
            }, py::arg("field"), "Add dimensioned value to tmp<volField>")

            .def("__sub__", [](const dimensioned<Type>& self, const GeometricField<Type, fvPatchField, volMesh>& field) {
                return self - field;
            }, py::arg("field"), "Subtract volField from dimensioned value")
            .def("__sub__", [](const dimensioned<Type>& self, const tmp<GeometricField<Type, fvPatchField, volMesh>>& field) {
                return self - field;
            }, py::arg("field"), "Subtract tmp<volField> from dimensioned value")
            ;
    }

}


void bindDimensioned(pybind11::module& m)
{
    namespace py = pybind11;

    // Bind dimensionSet class
    py::class_<Foam::dimensionSet>(m, "dimensionSet")
        .def(py::init<Foam::scalar, Foam::scalar, Foam::scalar, Foam::scalar, Foam::scalar, Foam::scalar, Foam::scalar>())
        .def("__pow__", [](const Foam::dimensionSet& self, Foam::scalar p) {
            return Foam::pow(self, p);
        })
        .def("__mul__", [](const Foam::dimensionSet& self, const Foam::dimensionSet& other) {
            return self * other;
        })
        .def("__truediv__", [](const Foam::dimensionSet& self, const Foam::dimensionSet& other) {
            return self / other;
        })
        .def("__and__", [](const Foam::dimensionSet& self, const Foam::dimensionSet& other) {
            return self & other;
        })
        ;

    // Bind common dimension constants
    m.attr("dimless") = Foam::dimless;
    m.attr("dimMass") = Foam::dimMass;
    m.attr("dimLength") = Foam::dimLength;
    m.attr("dimArea") = Foam::dimArea;
    m.attr("dimTime") = Foam::dimTime;
    m.attr("dimTemperature") = Foam::dimTemperature;
    m.attr("dimMoles") = Foam::dimMoles;
    m.attr("dimCurrent") = Foam::dimCurrent;
    m.attr("dimLuminousIntensity") = Foam::dimLuminousIntensity;
    m.attr("dimVelocity") = Foam::dimVelocity;
    m.attr("dimAcceleration") = Foam::dimAcceleration;
    m.attr("dimForce") = Foam::dimForce;
    m.attr("dimPressure") = Foam::dimPressure;
    m.attr("dimDensity") = Foam::dimDensity;
    m.attr("dimEnergy") = Foam::dimEnergy;
    m.attr("dimPower") = Foam::dimPower;
    m.attr("dimViscosity") = Foam::dimViscosity;

    auto dsf = Foam::declare_dimensioned<Foam::scalar>(m, "dimensionedScalar");
    auto dvf = Foam::declare_dimensioned<Foam::vector>(m, "dimensionedVector");
    auto dtf = Foam::declare_dimensioned<Foam::tensor>(m, "dimensionedTensor");
    auto dtsf = Foam::declare_dimensioned<Foam::symmTensor>(m, "dimensionedSymmTensor");

    // Add scalar-specific cross-type multiplication operators for dimensioned<scalar>
    // (scalar × vector/tensor fields, etc.)
    // dimensioned<scalar> × volVectorField
    dsf.def("__mul__", [](const Foam::dimensioned<Foam::scalar>& self, const Foam::volVectorField& field) {
        return self * field;
    }, "dimensioned × volVectorField")
    .def("__mul__", [](const Foam::dimensioned<Foam::scalar>& self, const Foam::tmp<Foam::volVectorField>& field) {
        return self * field;
    }, "dimensioned × tmp<volVectorField>")
    ;


}
