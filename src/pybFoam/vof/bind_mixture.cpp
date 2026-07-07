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

namespace nb = nanobind;

namespace Foam
{

void bindMixture(nb::module_ & m)
{
    nb::class_<immiscibleIncompressibleTwoPhaseMixture>(
        m, "immiscibleIncompressibleTwoPhaseMixture"
    )
    .def(
        nb::init<const volVectorField&, const surfaceScalarField&>(),
        nb::arg("U"), nb::arg("phi")
    )
    // Phase fraction fields (non-const references for in-place modification)
    .def("alpha1",
        [](immiscibleIncompressibleTwoPhaseMixture& self) -> volScalarField& {
            return self.alpha1();
        },
        nb::rv_policy::reference_internal)
    .def("alpha2",
        [](immiscibleIncompressibleTwoPhaseMixture& self) -> volScalarField& {
            return self.alpha2();
        },
        nb::rv_policy::reference_internal)
    // Phase densities (from incompressibleTwoPhaseMixture via twoPhaseMixture)
    .def("rho1",
        &immiscibleIncompressibleTwoPhaseMixture::rho1,
        nb::rv_policy::reference_internal)
    .def("rho2",
        &immiscibleIncompressibleTwoPhaseMixture::rho2,
        nb::rv_policy::reference_internal)
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
        nb::rv_policy::reference_internal)
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
