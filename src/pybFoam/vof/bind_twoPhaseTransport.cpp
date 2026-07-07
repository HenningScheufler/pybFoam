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

Description
    Bindings for incompressibleInterPhaseTransportModel instantiated with
    immiscibleIncompressibleTwoPhaseMixture.

    Exposed as TwoPhaseTransportModel in Python.

\*---------------------------------------------------------------------------*/

#include "bind_twoPhaseTransport.hpp"
#include "immiscibleIncompressibleTwoPhaseMixture.H"
#include "incompressibleInterPhaseTransportModel.H"
#include "volFields.H"
#include "surfaceFields.H"

namespace nb = nanobind;

// Concrete typedef for the template instantiation used by interFoam
typedef Foam::incompressibleInterPhaseTransportModel
<
    Foam::immiscibleIncompressibleTwoPhaseMixture
> TwoPhaseTransportModelType;

namespace Foam
{

void bindTwoPhaseTransport(nb::module_ & m)
{
    nb::class_<TwoPhaseTransportModelType>(m, "TwoPhaseTransportModel")
    .def(
        nb::init<
            const volScalarField&,
            const volVectorField&,
            const surfaceScalarField&,
            const surfaceScalarField&,
            const immiscibleIncompressibleTwoPhaseMixture&
        >(),
        nb::arg("rho"),
        nb::arg("U"),
        nb::arg("phi"),
        nb::arg("rhoPhi"),
        nb::arg("mixture"),
        nb::keep_alive<1, 2>(),  // keep rho alive
        nb::keep_alive<1, 3>(),  // keep U alive
        nb::keep_alive<1, 4>(),  // keep phi alive
        nb::keep_alive<1, 5>(),  // keep rhoPhi alive
        nb::keep_alive<1, 6>()   // keep mixture alive
    )
    .def("divDevRhoReff",
        &TwoPhaseTransportModelType::divDevRhoReff,
        nb::arg("rho"), nb::arg("U"))
    .def("correct",
        &TwoPhaseTransportModelType::correct)
    ;
}

} // End namespace Foam

// ************************************************************************* //
