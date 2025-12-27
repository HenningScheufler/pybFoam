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
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// OpenFOAM includes
#include "sampledSurface.H"
#include "sampledPlane.H"
#include "sampledPatch.H"
#include "sampledCuttingPlane.H"
#include "interpolation.H"

namespace py = pybind11;

namespace Foam
{
    void bindSampledSurface(py::module& m);
    void bindSampledPlane(py::module& m);
    void bindSampledPatch(py::module& m);
    void bindSampledCuttingPlane(py::module& m);
    void bindInterpolation(py::module& m);
    void bindSamplingFunctions(py::module& m);
}

#endif
