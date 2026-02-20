/*---------------------------------------------------------------------------*\
            Copyright (c) 2025, NeoFOAM authors
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

#include "bind_mixture.hpp"
#include "immiscibleIncompressibleTwoPhaseMixture.H"
#include "volFields.H"
#include "surfaceFields.H"

namespace py = pybind11;

namespace Foam
{

void bindMixture(py::module& m)
{
    py::class_<immiscibleIncompressibleTwoPhaseMixture>(
        m, "immiscibleIncompressibleTwoPhaseMixture"
    )
    .def(
        py::init<const volVectorField&, const surfaceScalarField&>(),
        py::arg("U"), py::arg("phi")
    )
    // Phase fraction fields (non-const references for in-place modification)
    .def("alpha1",
        [](immiscibleIncompressibleTwoPhaseMixture& self) -> volScalarField& {
            return self.alpha1();
        },
        py::return_value_policy::reference_internal)
    .def("alpha2",
        [](immiscibleIncompressibleTwoPhaseMixture& self) -> volScalarField& {
            return self.alpha2();
        },
        py::return_value_policy::reference_internal)
    // Phase densities (from incompressibleTwoPhaseMixture via twoPhaseMixture)
    .def("rho1",
        &immiscibleIncompressibleTwoPhaseMixture::rho1,
        py::return_value_policy::reference_internal)
    .def("rho2",
        &immiscibleIncompressibleTwoPhaseMixture::rho2,
        py::return_value_policy::reference_internal)
    // Mixture viscosity
    .def("nu",
        [](const immiscibleIncompressibleTwoPhaseMixture& self) {
            return self.nu();
        })
    // Interface properties (from interfaceProperties base)
    .def("cAlpha",
        &immiscibleIncompressibleTwoPhaseMixture::cAlpha)
    .def("nHatf",
        &immiscibleIncompressibleTwoPhaseMixture::nHatf,
        py::return_value_policy::reference_internal)
    .def("surfaceTensionForce",
        &immiscibleIncompressibleTwoPhaseMixture::surfaceTensionForce)
    // Correct transport and interface properties
    .def("correct",
        &immiscibleIncompressibleTwoPhaseMixture::correct)
    .def("read",
        &immiscibleIncompressibleTwoPhaseMixture::read)
    ;
}

} // End namespace Foam

// ************************************************************************* //
