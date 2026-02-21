/*---------------------------------------------------------------------------*\
            Copyright (c) 2025, Henning Scheufler
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

#ifndef pybFoam_meshing_bind_snappy_H
#define pybFoam_meshing_bind_snappy_H

#include <nanobind/nanobind.h>
#include "fvMesh.H"

namespace nb = nanobind;

namespace Foam
{
    // Bind snappyHexMesh functions
    void addSnappyBindings(nanobind::module_& m);

    // Core function to run snappyHexMesh phases
    void generate_snappy_hex_mesh
    (
        fvMesh& mesh,
        const dictionary& dict,
        bool overwrite = true,
        bool verbose = true
    );
}

#endif
