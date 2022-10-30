/*---------------------------------------------------------------------------*\
            Copyright (c) 20212, Henning Scheufler
-------------------------------------------------------------------------------
License
    This file is part of the ECI4FOAM source code library, which is an
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

#include "foam_fields.H"
#include "foam_primitives.H"
#include "instantList.H"


namespace py = pybind11;

namespace Foam
{

template<typename Type>
py::array_t<scalar> toNumpy(const Field<Type>& values)
{
    label nElements = values.size();
    label nComps = pTraits<Type>::nComponents;

    py::array_t<scalar, py::array::c_style> arr({nElements, nComps});
    auto ra = arr.mutable_unchecked();

    forAll(values,celli)
    {
        for (label i = 0; i < pTraits<Type>::nComponents; ++i)
        {
            ra(celli, i) = values[celli].component(i);
        }
    }

    return arr;
}

template< >
py::array_t<scalar> toNumpy<scalar>(const Field<scalar>& values)
{
    label nElements = values.size();

    py::array_t<scalar, py::array::c_style> arr(nElements);
    auto ra = arr.mutable_unchecked();

    forAll(values,celli)
    {
        ra(celli) = values[celli];
    }

    return arr;
}


template<typename Type>
void fromNumpy(Field<Type>& values,const py::array_t<scalar> np_arr)
{
    label nComps = pTraits<Type>::nComponents;
    label nElements = values.size();

    if (np_arr.shape(1) != nComps)
    {
        FatalErrorInFunction
            << "dimensions do not match " << nl
            << "the expected value: " << nComps << nl
            << "the provided value: " << np_arr.shape(1) << nl
            << exit(FatalError);
    }

    if (np_arr.shape(0) != nElements)
    {
        FatalErrorInFunction
            << "length of numpy array does not match " << nl
            << "the expected value: " << nElements<< nl
            << "the provided value: " << np_arr.size() << nl
            << exit(FatalError);
    }
    const auto ra = np_arr.unchecked();

    forAll(values,celli)
    {
        for (label i = 0; i < pTraits<Type>::nComponents; ++i)
        {
            values[celli].component(i) = ra(celli, i); 
        }
    }
}

template<>
void fromNumpy<scalar>(Field<scalar>& values,const py::array_t<scalar> np_arr)
{
    if (np_arr.ndim() != 1)
    {
        FatalErrorInFunction
            << "numpy array is not onedimensional " << nl
            << "provided array has ndims : " << np_arr.ndim() << nl
            << exit(FatalError);
    }

    if (np_arr.size() != values.size())
    {
        FatalErrorInFunction
            << "length of numpy array does not match " << nl
            << "the expected value: " << values.size()<< nl
            << "the provided value: " << np_arr.size() << nl
            << exit(FatalError);
    }

    const auto ra = np_arr.unchecked();

    forAll(values,celli)
    {
        values[celli] = ra(celli); 
    }
}

template<class Type>
Type declare_sum(const Field<Type>& values)
{
    return gSum(values);
}



template<class Type>
py::class_< Field<Type>> declare_fields(py::module &m, std::string className) {
    auto fieldClass = py::class_< Field<Type>>(m, className.c_str())
    .def(py::init<>())
    .def(py::init< Field<Type>>())
    .def(py::init< tmp<Field<Type> >>())
    .def(py::init([](std::vector<Type> vec) {
        Field<Type> f(vec.size());
        forAll(f,i)
        {
            f[i] = vec[i];
        }
        return f;
    }))
    .def("__len__", [](const Field<Type>& self) {
        return self.size();
    })
    .def("__getitem__", [](const Field<Type>& self, const label idx) {
        if (idx >= self.size())
        {
            throw py::index_error();
        }
        return self[idx];
    })
    .def("__setitem__", [](Field<Type>& self, const label idx,const Type& s) {
        self[idx] = s;
    })
    .def("__add__", &Foam::add<Field<Type> >)
    .def("__add__", [](Field<Type>& self, const Type& s) {return Field<Type>(self + s);})
    .def("__sub__", &Foam::subtract<Field<Type> >)
    .def("__sub__", [](Field<Type>& self, const Type& s) {return Field<Type>(self + s);})
    .def("__mul__", [](Field<Type>& self, const scalar& s) {return Field<Type>(self * s);})
    .def("__mul__", [](Foam::Field<Type>& self, const Field<scalar>& sf)
    {
        return Field<Type>(self * sf);
    })
    .def("__truediv__", [](Field<Type>& self, const scalar& s) {return Field<Type>(self / s);})
    .def("__truediv__", [](Field<Type>& self, const Field<scalar>& sf)
    {
        return Field<Type>(self * sf);
    })
    .def("to_numpy",&toNumpy<Type>)
    .def("from_numpy",&toNumpy<Type>)

    ;
    return fieldClass;
}

}

void AddFoamFields(py::module& m)
{
    py::class_<Foam::instantList>(m, "instantList")
        .def("__getitem__", [](const Foam::instantList& self, const Foam::label idx) {
            if (idx >= self.size())
            {
                throw py::index_error();
            }
            return self[idx];
        })
    ;

    py::class_<Foam::List<Foam::word>>(m, "wordList")
        .def(py::init<Foam::List<Foam::word> > ())
        .def(py::init([](std::vector<std::string> vec) {
            Foam::List<Foam::word> f(vec.size());
            forAll(f,i)
            {
                f[i] = vec[i];
            }
            return f;
        }))
        .def("__len__", [](const Foam::List<Foam::word>& self) {
            return self.size();
        })
        .def("__getitem__", [](const Foam::List<Foam::word>& self, const Foam::label idx) {
            if (idx >= self.size())
            {
                throw py::index_error();
            }
            return std::string(self[idx]);
        })
        .def("__setitem__", [](Foam::List<Foam::word>& self, const Foam::label idx,const std::string& s) {
            self[idx] = s;
        })
        .def("list",[](Foam::List<Foam::word>& self){
            std::vector<std::string> l_out(self.size());
            forAll(self,i)
            {
                l_out[i] = self[i];
            }
            return l_out;
        })
        ;


    auto sf = Foam::declare_fields<Foam::scalar>(m, std::string("scalarField"));

    auto vf = Foam::declare_fields<Foam::vector>(m, std::string("vectorField"));

    auto tf = Foam::declare_fields<Foam::tensor>(m, std::string("tensorField"));

    auto stf = Foam::declare_fields<Foam::symmTensor>(m, std::string("symmTensorField"));


    m.def("sum",Foam::declare_sum<Foam::scalar>);
    m.def("sum",Foam::declare_sum<Foam::vector>);
    m.def("sum",Foam::declare_sum<Foam::tensor>);
    m.def("sum",Foam::declare_sum<Foam::symmTensor>);
}
