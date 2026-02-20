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
    Python binding module for VoF (Volume of Fluid) functionality.

    Provides:
      - immiscibleIncompressibleTwoPhaseMixture (mixture model)
      - TwoPhaseTransportModel (turbulence for two-phase flows)
      - solveAlpha (MULES-based alpha equation with subcycling)
      - computeAlphaCourantNumber

\*---------------------------------------------------------------------------*/

#include <pybind11/pybind11.h>

#include "bind_mixture.hpp"
#include "bind_twoPhaseTransport.hpp"
#include "bind_alphaEqn.hpp"

namespace py = pybind11;


PYBIND11_MODULE(vof, m)
{
    m.doc() = "pybFoam VoF (Volume of Fluid) bindings for two-phase flows";

    Foam::bindMixture(m);
    Foam::bindTwoPhaseTransport(m);
    Foam::bindAlphaEqn(m);
}

// ************************************************************************* //
