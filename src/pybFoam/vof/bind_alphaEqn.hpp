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

#ifndef bind_alphaEqn_H
#define bind_alphaEqn_H

#include <pybind11/pybind11.h>
#include "immiscibleIncompressibleTwoPhaseMixture.H"
#include "volFields.H"
#include "surfaceFields.H"

namespace Foam
{

// Public API: run a single pass of the alpha equation (MULES-based)
// May be called directly from Python via the 'run_alpha_eqn' binding.
void runAlphaEqn
(
    volScalarField& alpha1,
    volScalarField& alpha2,
    const surfaceScalarField& phi,
    surfaceScalarField& alphaPhi10,
    immiscibleIncompressibleTwoPhaseMixture& mixture,
    int  nAlphaCorr,
    bool MULESCorr,
    const word& alphaScheme,
    const word& alpharScheme
);

void bindAlphaEqn(pybind11::module& m);

} // End namespace Foam

#endif

// ************************************************************************* //
