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
#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/make_iterator.h>

#include <string>

#include "Field.H"
#include "scalar.H"
#include <nanobind/stl/vector.h>
#include <nanobind/ndarray.h>

namespace nb = nanobind;

namespace Foam
{


template<typename Type>
nb::ndarray<nb::numpy, scalar> toNumpy(const Field<Type>& values);

template< >
nb::ndarray<nb::numpy, scalar> toNumpy<scalar>(const Field<scalar>& values);

template<class Type>
Type declare_sum(const Field<Type>& values);

template<typename Type>
void fromNumpy(Field<Type>& values, nb::ndarray<nb::numpy, scalar, nb::ndim<1>> np_arr);

template<>
void fromNumpy<scalar>(Field<scalar>& values, nb::ndarray<nb::numpy, scalar, nb::ndim<1>> np_arr);

template<class Type>
nb::class_< Field<Type>> declare_fields(nb::module_ &m, std::string &className);

template<class Type>
nb::class_<tmp<Field<Type>>> declare_tmp_fields(nb::module_ &m, std::string className);

void  bindFields(nb::module_& m);

}

#endif
