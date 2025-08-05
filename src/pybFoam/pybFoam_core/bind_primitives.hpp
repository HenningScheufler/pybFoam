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

#ifndef foam_primitive
#define foam_primitive

// System includes
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "word.H"
#include "scalar.H"
#include "vector.H"
#include "tensor.H"
#include "symmTensor.H"

namespace py = pybind11;

namespace Foam
{

// arthemtic operators
template<class Type>
Type add(const Type& t1, const Type& t2) {
    return t1 + t2;
}

template<class Type>
Type subtract(const Type& t1, const Type& t2) {
    return t1 - t2;
}

template<class Type>
Type multiply(const Type& t1, const Type& t2) {
    return t1 * t2;
}

template<class Type>
Type divide(const Type& t1, const Type& t2) {
    return t1 / t2;
}

template<class Type>
Type multiply_scalar(const Type& t1, const scalar& s) {
    return t1 * s;
}

template<class Type>
typename innerProduct<Type, Type>::type inner_product(const Type& t1, const Type& t2) {
    return t1 & t2;
}

template<class Type, class Type2>
typename innerProduct<Type, Type2>::type inner_product(const Type& t1, const Type2& t2) {
    return t1 & t2;
}


template<class Type>
bool is_equal(const Type& t1, const Type& t2) {
    return t1 == t2;
}

template<class Type>
bool is_notequal(const Type& t1, const Type& t2) {
    return t1 != t2;
}



// comparision operators
template<class Type>
bool is_equal(const Type& t1, const Type& t2);

template<class Type>
bool is_notequal(const Type& t1, const Type& t2);

template<class Type>
py::class_<Type> declare_vectorspace(py::module &m, std::string &className);



}


void  bindPrimitives(py::module& m);


#endif 
