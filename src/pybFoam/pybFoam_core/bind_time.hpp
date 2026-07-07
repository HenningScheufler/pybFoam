/*---------------------------------------------------------------------------*\
            Copyright (c) 2022, Henning Scheufler
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
    Foam::bindTime

Description
    Python bindings for OpenFOAM Time class and related utilities

Author
    Henning Scheufler, all rights reserved.

\*---------------------------------------------------------------------------*/

#ifndef foam_bind_time_hpp
#define foam_bind_time_hpp

// System includes
#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <vector>

// OpenFOAM includes
#include "Time.H"
#include "argList.H"
#include "instantList.H"
#include "timeSelector.H"


namespace Foam
{
    // Time-related utility functions
    Foam::instantList selectTimes(
        Time& runTime,
        const std::vector<std::string>& args
    );

    void createTime(Time* self, const std::string& rootPath, const std::string& caseName);

    void createTimeArgs(Time* self, const argList& args);

    void makeArgList(argList* self, const std::vector<std::string>& args);
}

// Binding function
void bindTime(nanobind::module_& m);


#endif // foam_bind_time_hpp
