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

namespace py = pybind11;

// Concrete typedef for the template instantiation used by interFoam
typedef Foam::incompressibleInterPhaseTransportModel
<
    Foam::immiscibleIncompressibleTwoPhaseMixture
> TwoPhaseTransportModelType;

namespace Foam
{

void bindTwoPhaseTransport(py::module& m)
{
    py::class_<TwoPhaseTransportModelType>(m, "TwoPhaseTransportModel")
    .def(
        py::init<
            const volScalarField&,
            const volVectorField&,
            const surfaceScalarField&,
            const surfaceScalarField&,
            const immiscibleIncompressibleTwoPhaseMixture&
        >(),
        py::arg("rho"),
        py::arg("U"),
        py::arg("phi"),
        py::arg("rhoPhi"),
        py::arg("mixture"),
        py::keep_alive<1, 2>(),  // keep rho alive
        py::keep_alive<1, 3>(),  // keep U alive
        py::keep_alive<1, 4>(),  // keep phi alive
        py::keep_alive<1, 5>(),  // keep rhoPhi alive
        py::keep_alive<1, 6>()   // keep mixture alive
    )
    .def("divDevRhoReff",
        &TwoPhaseTransportModelType::divDevRhoReff,
        py::arg("rho"), py::arg("U"))
    .def("correct",
        &TwoPhaseTransportModelType::correct)
    ;
}

} // End namespace Foam

// ************************************************************************* //
