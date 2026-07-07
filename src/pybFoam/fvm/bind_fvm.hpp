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
    Foam::bind_fvm

Description

Author
    Henning Scheufler, all rights reserved.

SourceFiles


\*---------------------------------------------------------------------------*/

#ifndef bind_fvm
#define bind_fvm

// System includes
#include <nanobind/nanobind.h>

namespace Foam
{
    namespace nb = nanobind;

    template<class Type>
    void bindFvmDdt(nb::module_& fvm);

    template<class Type>
    void bindFvmDiv(nb::module_& fvm);

    template<class Type>
    void bindFvmLaplacian(nb::module_& fvm);



    void  bindFVM(nb::module_& m);
}

#endif // bind_fvm defined
