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

Class
    Foam::bind_mules

Description
    Generic MULES (Multidimensional Universal Limiter for Explicit Solution)
    bindings.  Not VoF-specific — usable for any bounded scalar transport.

\*---------------------------------------------------------------------------*/

#ifndef bind_mules
#define bind_mules

#include <nanobind/nanobind.h>

namespace Foam
{
    void bindMULES(nanobind::module_& m);
}

#endif // bind_mules defined
