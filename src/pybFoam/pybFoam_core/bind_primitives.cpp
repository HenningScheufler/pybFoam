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

#include "bind_primitives.hpp"

#include "instant.H"


namespace Foam
{
namespace py = pybind11;

template<class Type>
py::class_<Type> declare_vectorspace(py::module &m, std::string className) {
    auto primitiveType = py::class_<Type>(m, className.c_str())
        .def(py::init<Type>())
        .def(py::init([](const std::array<Foam::scalar, pTraits<Type>::nComponents>& v){
            Type val = Zero;
            for(int i = 0;i<pTraits<Type>::nComponents;i++ )
            {
                val[i] = v[i];
            }
            return val;
        }))
        .def("__getitem__", [](const Type& self, const Foam::label idx) {
            if (idx >= pTraits<Type>::nComponents || idx < 0)
            {
                throw py::index_error();
            }
            return self[idx];
        })
        .def("__setitem__", [](Type& self, const Foam::label idx,const Foam::scalar s) {
            if (idx >= pTraits<Type>::nComponents || idx < 0)
            {
                throw py::index_error();
            }
            self[idx] = s;
        })
        .def("__str__", [](const Type& self)
        {
            std::string out = "(" + Foam::name(self[0]);
            for (label i = 1; i < pTraits<Type>::nComponents; ++i)
            {
                out += " " +  Foam::name(self[i]);
            }
            out += ")";
            return out;
        })
        .def("__len__", [](Type& self) -> int {return pTraits<Type>::nComponents;})
        .def("__add__", &Foam::add<Type>)
        .def("__sub__", &Foam::subtract<Type>)
        .def("__mul__", &Foam::multiply_scalar<Type>)
        .def("__and__", &Foam::inner_product<Type>)
        .def("__eq__", &Foam::is_equal<Type>)
        .def("__eq__",[](const Type& self, const std::array<Foam::scalar, pTraits<Type>::nComponents>& v){
            for(int i = 0;i<pTraits<Type>::nComponents;i++ )
            {
                if (self[i] != v[i])
                {
                    return false;
                }
            }
            return true;
        })
        .def("__ne__", &Foam::is_notequal<Type>)
        ;
    return primitiveType;
}

}

void bindPrimitives(pybind11::module& m)
{
    namespace py = pybind11;

    
    py::class_<Foam::instant>(m, "instant")
        .def("__str__",[](const Foam::instant& self){return std::string(self.name());})
    ;


    // primitive classes
    py::class_<Foam::word>(m, "Word")
        .def(py::init<std::string>())
        .def("__eq__",[](const Foam::word& self, std::string w){
            return bool(self == w);
        })
        .def("__str__",[](const Foam::word& self){
            return std::string(self);
        })
        ;


    auto vector = Foam::declare_vectorspace<Foam::vector>(m, std::string("vector"));
        vector
            .def(py::init<Foam::scalar,Foam::scalar,Foam::scalar>())
            .def("__and__",&Foam::inner_product<Foam::vector,Foam::tensor>)
            .def("__and__",&Foam::inner_product<Foam::vector,Foam::symmTensor>);

    auto tensor = Foam::declare_vectorspace<Foam::tensor>(m, std::string("tensor"));
        tensor
            .def(py::init<Foam::scalar,Foam::scalar,Foam::scalar,
                          Foam::scalar,Foam::scalar,Foam::scalar,
                          Foam::scalar,Foam::scalar,Foam::scalar>())
            .def("__and__",&Foam::inner_product<Foam::tensor,Foam::vector>);

    auto symmTensor = Foam::declare_vectorspace<Foam::symmTensor>(m, std::string("symmTensor"));
        symmTensor
            .def(py::init<Foam::scalar,Foam::scalar,Foam::scalar,
                          Foam::scalar,Foam::scalar,Foam::scalar>())
            .def("__and__",&Foam::inner_product<Foam::symmTensor,Foam::vector>);


    // functions
    m.def("mag", [](Foam::scalar t){return Foam::mag(t);});
    m.def("mag", [](Foam::vector t){return Foam::mag(t);});
    m.def("mag", [](Foam::tensor t){return Foam::mag(t);});
}
