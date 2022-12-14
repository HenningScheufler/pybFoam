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

#ifndef foam_fields
#define foam_fields

// System includes
#include <pybind11/pybind11.h>
#include "Field.H"
#include "scalar.H"
#include <pybind11/stl.h>
#include <pybind11/numpy.h>


namespace py = pybind11;

namespace Foam
{


template<typename Type>
py::array_t<scalar> toNumpy(const Field<Type>& values);

template< >
py::array_t<scalar> toNumpy<scalar>(const Field<scalar>& values);

template<class Type>
Type declare_sum(const Field<Type>& values);

template<typename Type>
void fromNumpy(Field<Type>& values,const py::array_t<scalar> np_arr);

template<>
void fromNumpy<scalar>(Field<scalar>& values,const py::array_t<scalar> np_arr);

template<class Type>
py::class_< Field<Type>> declare_fields(py::module &m, std::string &className);

void  AddFoamFields(py::module& m);

}

#endif // foam_dict  defined 
