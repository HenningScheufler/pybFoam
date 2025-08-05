/*---------------------------------------------------------------------------*\
            Copyright (c) 20212, Henning Scheufler
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

#include "bind_dimensioned.hpp"
#include "dimensionedType.H"

namespace Foam
{

    template<class Type>
    void declare_dimensioned(py::module &m, std::string className)
    {
        py::class_<dimensioned<Type>>(m, className.c_str())
            .def(py::init<const word&,  const dimensionSet, const Type&>())
            // .def("name", &dimensioned<Type>::name, py::return_value_policy::reference)
            // .def("dimensions", &dimensioned<Type>::dimensions)
            // .def("value", &dimensioned<Type>::value, py::return_value_policy::reference)
            ;
    }

}


void bindDimensioned(pybind11::module& m)
{

    Foam::declare_dimensioned<Foam::scalar>(m, "DimensionedScalarField");
    Foam::declare_dimensioned<Foam::vector>(m, "DimensionedVectorField");
    Foam::declare_dimensioned<Foam::tensor>(m, "DimensionedTensorField");

}
