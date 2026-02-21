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

template<class Type>
bool eq_type_or_sequence(const Type& self, nb::object other)
{
    constexpr int N = pTraits<Type>::nComponents;

    // Case 1: Type == Type
    if (nb::isinstance<Type>(other)) {
        const Type& rhs = nb::cast<const Type&>(other);
        return Foam::is_equal<Type>(self, rhs);
    }

    // Case 2: Type == (list/tuple) with N numeric items
    if (nb::isinstance<nb::list>(other) || nb::isinstance<nb::tuple>(other))
    {
        nb::object seq = other;
        size_t sz = nb::len(seq);
        if ((int)sz != N) {
            return false;   // common Python behavior: different size => not equal
        }

        for (int i = 0; i < N; ++i) {
            // If element isn't convertible to scalar, we can't compare
            // (could throw; better to NotImplemented)
            try {
                Foam::scalar v = nb::cast<Foam::scalar>(seq.attr("__getitem__")(i));
                if (self[i] != v) return false;
            } catch (const nb::cast_error&) {
                return false;
            }
        }
        return true;
    }
    return false;

}

template<class Type>
nb::class_<Type> declare_vectorspace(nb::module_ &m, std::string className) {
    auto primitiveType = nb::class_<Type>(m, className.c_str())
        .def("__init__", [](Type* self, const Type& other) {
            new (self) Type(other);
        })
        .def("__init__", [](Type* self, const std::vector<Foam::scalar>& v) {
            new (self) Type(Zero);
            for(int i = 0; i < pTraits<Type>::nComponents; ++i)
            {
                (*self)[i] = v[i];
            }
        })
        .def("__init__", [](Type* self, nb::tuple t) {
            if ((int)t.size() != pTraits<Type>::nComponents)
                throw std::runtime_error(
                    "Expected tuple of length " + std::to_string(pTraits<Type>::nComponents));
            new (self) Type(Zero);
            for (int i = 0; i < pTraits<Type>::nComponents; ++i)
                (*self)[i] = nb::cast<Foam::scalar>(t[i]);
        })
        .def("__getitem__", [](const Type& self, const Foam::label idx) {
            if (idx >= pTraits<Type>::nComponents || idx < 0)
            {
                throw nb::index_error();
            }
            return self[idx];
        })
        .def("__setitem__", [](Type& self, const Foam::label idx,const Foam::scalar s) {
            if (idx >= pTraits<Type>::nComponents || idx < 0)
            {
                throw nb::index_error();
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
        .def("__eq__", [](const Type& self, nb::object other) {
            return eq_type_or_sequence<Type>(self, std::move(other));
        })
        .def("__ne__", [](const Type& self, nb::object other) {
            return !eq_type_or_sequence<Type>(self, std::move(other));
        })
        .def("__hash__", [](const Type& self) {
            size_t seed = 0;
            for (int i = 0; i < pTraits<Type>::nComponents; ++i) {
                seed ^= std::hash<double>{}(self[i]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        })
        ;
    return primitiveType;
}

template<class Type, int nComponents>
nb::class_<Type> declare_int_vectorspace(nb::module_ &m, std::string className) {
    return nb::class_<Type>(m, className.c_str())
        .def("__getitem__", [](const Type& self, const Foam::label idx) {
            if (idx >= nComponents || idx < 0) throw nb::index_error();
            return self[idx];
        })
        .def("__setitem__", [](Type& self, const Foam::label idx, int val) {
            if (idx >= nComponents || idx < 0) throw nb::index_error();
            self[idx] = val;
        })
        .def("__len__", [](const Type&) { return nComponents; });
}

}

void bindPrimitives(nanobind::module_& m)
{

    nb::class_<Foam::instant>(m, "instant")
        .def("__str__",[](const Foam::instant& self){return std::string(self.name());})
    ;


    // primitive classes
    nb::class_<Foam::word>(m, "Word")
        .def(nb::init<Foam::word>())
        .def(nb::init<std::string>())
        // .def("__eq__",[](const Foam::word& self, const Foam::word& w){
        //     return bool(self == w);
        // })
        .def("__eq__",
            [](const Foam::word& self, nb::object other) -> bool {
                // Word == Word
                if (nb::isinstance<Foam::word>(other)) {
                    const auto& rhs = nb::cast<const Foam::word&>(other);
                    return self == rhs;
                }
                // Word == str
                if (nb::isinstance<nb::str>(other)) {
                    std::string rhs = nb::cast<std::string>(other);
                    return self == rhs;
                }
                return false;
            }
        )
        // .def("__eq__",[](const Foam::word& self, const std::string& w){
        //     return bool(self == w);
        // })
        .def("__str__",[](const Foam::word& self){
            return std::string(self);
        })
        .def("__repr__",[](const Foam::word& self){
            return std::string(self);
        })
        .def("__hash__", [](const Foam::word& self) {
            return std::hash<std::string>{}(std::string(self));
        })
        ;
    nb::implicitly_convertible<std::string, Foam::word>();


    auto vector = Foam::declare_vectorspace<Foam::vector>(m, std::string("vector"));
        vector
            .def("__init__", [](Foam::vector* self, Foam::scalar x, Foam::scalar y, Foam::scalar z) {
                new (self) Foam::vector(x, y, z);
            })
            .def("__and__",&Foam::inner_product<Foam::vector,Foam::tensor>)
            .def("__and__",&Foam::inner_product<Foam::vector,Foam::symmTensor>);

    auto tensor = Foam::declare_vectorspace<Foam::tensor>(m, std::string("tensor"));
        tensor
            .def("__init__", [](Foam::tensor* self, Foam::scalar x1, Foam::scalar x2, Foam::scalar x3,
                                                 Foam::scalar x4, Foam::scalar x5, Foam::scalar x6,
                                                 Foam::scalar x7, Foam::scalar x8, Foam::scalar x9) {
                new (self) Foam::tensor(x1, x2, x3, x4, x5, x6, x7, x8, x9);
            })
            .def("__and__",&Foam::inner_product<Foam::tensor,Foam::vector>);

    auto symmTensor = Foam::declare_vectorspace<Foam::symmTensor>(m, std::string("symmTensor"));
        symmTensor
            .def("__init__", [](Foam::symmTensor* self, Foam::scalar x1, Foam::scalar x2, Foam::scalar x3,
                                                     Foam::scalar x4, Foam::scalar x5, Foam::scalar x6) {
                new (self) Foam::symmTensor(x1, x2, x3, x4, x5, x6);
            })
            .def("__and__",&Foam::inner_product<Foam::symmTensor,Foam::vector>);

    // Integer tensor types - simple bindings
    Foam::declare_int_vectorspace<Foam::Vector<int>, 3>(m, "VectorInt");
    Foam::declare_int_vectorspace<Foam::Tensor<int>, 9>(m, "TensorInt");
    Foam::declare_int_vectorspace<Foam::SymmTensor<int>, 6>(m, "SymmTensorInt");

    // functions
    m.def("mag", [](Foam::scalar t){return Foam::mag(t);});
    m.def("mag", [](Foam::vector t){return Foam::mag(t);});
    m.def("mag", [](Foam::tensor t){return Foam::mag(t);});
}
