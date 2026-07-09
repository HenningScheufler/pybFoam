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
    Binding for Foam::isoAdvection — geometric VoF advection (interIsoFoam).

    isoAdvection reconstructs the interface (reconstructionScheme, e.g. isoAlpha)
    and advects alpha1 in place, reading its controls from the case's
    fvSolution ``solvers.<alpha1>`` sub-dict. The advector is constructed once
    (mirroring interIsoFoam's createFields.H) and ``advect()`` is called each
    outer corrector.

\*---------------------------------------------------------------------------*/

#include "bind_isoAdvection.hpp"

#include "isoAdvection.H"
#include "zeroField.H"
#include "volFields.H"
#include "surfaceFields.H"

namespace nb = nanobind;

namespace Foam
{

void bindIsoAdvection(nb::module_ & m)
{
    nb::class_<isoAdvection>(m, "isoAdvection")
    .def(
        nb::init<volScalarField&, const surfaceScalarField&, const volVectorField&>(),
        nb::arg("alpha1"), nb::arg("phi"), nb::arg("U"),
        // The advector stores references to alpha1, phi and U; keep them alive
        // for as long as the advector is alive.
        nb::keep_alive<1, 2>(), nb::keep_alive<1, 3>(), nb::keep_alive<1, 4>()
    )
    // Advect the interface (updates alpha1 in place). The zeroField Sp/Su match
    // interFoam's alphaSuSp.H for the no-phase-change case; instantiating the
    // template here emits its body (declared in isoAdvectionTemplates.C).
    .def("advect",
        [](isoAdvection& self) { self.advect(zeroField(), zeroField()); })
    // Density-weighted face flux rhoPhi = (rho1-rho2)*alphaPhi + rho2*phi.
    // Materialise the returned tmp into a value so it survives the crossing
    // into Python.
    .def("get_rho_phi",
        [](const isoAdvection& self, const dimensionedScalar& rho1,
           const dimensionedScalar& rho2)
        {
            return surfaceScalarField(self.getRhoPhi(rho1, rho2));
        },
        nb::arg("rho1"), nb::arg("rho2"))
    // Bounded phase face-flux alphaPhi (kept internally by the advector).
    .def("alpha_phi",
        [](const isoAdvection& self) -> const surfaceScalarField& {
            return self.alphaPhi();
        },
        nb::rv_policy::reference_internal)
    ;
}

} // End namespace Foam

// ************************************************************************* //
