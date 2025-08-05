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
    Foam::pyInterp

Description

Author
    Henning Scheufler, all rights reserved.

SourceFiles


\*---------------------------------------------------------------------------*/

#ifndef foam_fvMatrix
#define foam_fvMatrix

// System includes
#include <pybind11/pybind11.h>

#include "fvMatrix.H"

namespace py = pybind11;

namespace Foam
{

template<class Type>
py::class_< fvMatrix<Type>>
declare_fvMatrix(py::module &m, std::string className);

template<class Type>
void declare_solve(py::module &m);

void  bindFvMatrix(pybind11::module& m);

}

#endif // foam_geo_fields  defined 
