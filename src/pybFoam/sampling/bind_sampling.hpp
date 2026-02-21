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

Description
    Python bindings for OpenFOAM sampledSurface functionality

\*---------------------------------------------------------------------------*/

#ifndef bind_sampling_hpp
#define bind_sampling_hpp

// System includes
#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/shared_ptr.h>

// OpenFOAM includes
#include "sampledSurface.H"
#include "sampledPlane.H"
#include "sampledPatch.H"
#include "sampledCuttingPlane.H"
#include "sampledIsoSurface.H"
#include "sampledSet.H"
#include "coordSet.H"
#include "meshSearch.H"
#include "interpolation.H"

namespace nb = nanobind;

namespace Foam
{
    void bindSampledSurface(nb::module_& m);
    void bindSampledSet(nb::module_& m);
    void bindMeshSearch(nb::module_& m);
    void bindInterpolation(nb::module_& m);
    void bindSamplingFunctions(nb::module_& m);
}

#endif
