/*---------------------------------------------------------------------------*\
            Copyright (c) 2026, NeoFOAM authors
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
    Generic MULES bindings: explicit_solve and correct with unit bounds
    (0..1) and zero source terms.  These mirror MULES::explicitSolve /
    MULES::correct as used by bounded scalar transport (e.g. VoF alpha),
    but carry no VoF-specific state and live in their own module so they can
    be composed from Python.

\*---------------------------------------------------------------------------*/

#include "bind_mules.hpp"

#include "CMULES.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "geometricOneField.H"
#include "zeroField.H"

namespace nb = nanobind;

namespace Foam
{

void bindMULES(nb::module_& m)
{
    // MULES::explicitSolve(1, psi, phi, psiPhi, 0, 0, 1, 0)
    // Updates psi and psiPhi in-place.
    m.def(
        "explicit_solve",
        [](volScalarField& psi,
           const surfaceScalarField& phi,
           surfaceScalarField& psiPhi)
        {
            MULES::explicitSolve
            (
                geometricOneField(),
                psi,
                phi,
                psiPhi,
                zeroField(),
                zeroField(),
                oneField(),
                zeroField()
            );
        },
        nb::arg("psi"), nb::arg("phi"), nb::arg("psi_phi"),
        "MULES explicit solve with unit bounds and zero sources.\n"
        "Equivalent to MULES::explicitSolve(1, psi, phi, psi_phi, 0, 0, 1, 0).\n"
        "Updates psi and psi_phi in-place."
    );

    // MULES::correct(1, psi, psiPhiUn, psiPhiCorr, 0, 0, 1, 0)
    // psiPhiCorr must be initialised to (psiPhiUn - psiPhi0) by the caller.
    // Updates psi and psiPhiCorr in-place.
    m.def(
        "correct",
        [](volScalarField& psi,
           const surfaceScalarField& psiPhiUn,
           surfaceScalarField& psiPhiCorr)
        {
            MULES::correct
            (
                geometricOneField(),
                psi,
                psiPhiUn,
                psiPhiCorr,
                zeroField(),
                zeroField(),
                oneField(),
                zeroField()
            );
        },
        nb::arg("psi"), nb::arg("psi_phi_un"), nb::arg("psi_phi_corr"),
        "MULES correction step with unit bounds and zero sources.\n"
        "psi_phi_corr must be initialised to (psi_phi_un - psi_phi0) before the call;\n"
        "afterwards it holds the limited correction flux.\n"
        "Equivalent to MULES::correct(1, psi, psi_phi_un, psi_phi_corr, 0, 0, 1, 0).\n"
        "Updates psi and psi_phi_corr in-place."
    );
}

} // End namespace Foam
