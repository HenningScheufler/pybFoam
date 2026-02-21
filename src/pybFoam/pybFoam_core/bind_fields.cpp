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

#include "bind_fields.hpp"
#include "bind_primitives.hpp"
#include "instantList.H"
#include "uniformDimensionedFields.H"
#include "fvMesh.H"
#include "volFields.H"
#include "surfaceFields.H"


namespace nb = nanobind;

namespace Foam
{

template<class Type>
Type declare_sum(const Field<Type>& values)
{
    return gSum(values);
}


template<class Type>
nb::class_< Field<Type>> declare_fields(nb::module_ &m, std::string className) {
    auto fieldClass = nb::class_< Field<Type>>(m, className.c_str())
    .def(nb::init<>())
    .def("__init__", [](Field<Type>* self, const Field<Type>& other) {
        new (self) Field<Type>(other);
    })
    .def("__init__", [](Field<Type>* self, const tmp<Field<Type>>& t) {
        new (self) Field<Type>(t());
    })
    .def("__init__", [](Field<Type>* self, std::vector<Type> vec) {
        new (self) Field<Type>(vec.size());
        forAll(*self, i)
        {
            (*self)[i] = vec[i];
        }
    })
    .def("__init__", [](Field<Type>* self, nb::ndarray<nb::numpy, Foam::scalar, nb::device::cpu> arr) {
        constexpr bool isScalar = std::is_same<Type, Foam::scalar>::value;
        constexpr int nComps = isScalar ? 1 : Foam::pTraits<Type>::nComponents;

        if (arr.ndim() != (size_t)(isScalar ? 1 : 2))
            throw std::runtime_error(
                "Expected " + std::to_string(isScalar ? 1 : 2) + "D array for this field type");

        if (!isScalar && (int)arr.shape(1) != nComps)
            throw std::runtime_error(
                "Expected second dimension to be " + std::to_string(nComps)
            );

        size_t n = arr.shape(0);
        const Foam::scalar* data = arr.data();

        new (self) Foam::Field<Type>(n);

        if constexpr (isScalar) {
            for (size_t i = 0; i < n; ++i)
            {
                (*self)[i] = data[i];
            }
        }
        else
        {
            for (size_t i = 0; i < n; ++i) {
                Type val;
                for (int j = 0; j < nComps; ++j)
                    val[j] = data[i * nComps + j];
                (*self)[i] = val;
            }
        }
    })
    .def("__len__", [](const Field<Type>& self) {
        return self.size();
    })
    .def("__getitem__", [](const Field<Type>& self, const label idx) {
        if (idx >= self.size())
        {
            throw nb::index_error();
        }
        return self[idx];
    })
    .def("__setitem__", [](Field<Type>& self, const label idx,const Type& s) {
        self[idx] = s;
    })
    .def("__add__", [](const Field<Type>& self, const Field<Type>& f) {
        return self + f;
    })
    .def("__add__", [](const Field<Type>& self, const tmp<Field<Type>>& f) {
        return self + f();
    })
    .def("__add__", [](Field<Type>& self, const Type& s) {return self + s;})
    .def("__sub__", [](const Field<Type>& self, const Field<Type>& f) {
        return self - f;
    })
    .def("__sub__", [](const Field<Type>& self, const tmp<Field<Type>>& f) {
        return self - f();
    })
    .def("__sub__", [](Field<Type>& self, const Type& s) {return self - s;})
    .def("__iadd__", [](Field<Type>& self, const Field<Type>& f) -> Field<Type>& {
        self += f;
        return self;
    }, nb::rv_policy::reference_internal)
    .def("__iadd__", [](Field<Type>& self, const tmp<Field<Type>>& f) -> Field<Type>& {
        self += f();
        return self;
    }, nb::rv_policy::reference_internal)
    .def("__iadd__", [](Field<Type>& self, const Type& s) -> Field<Type>& {
        self += s;
        return self;
    }, nb::rv_policy::reference_internal)
    .def("__isub__", [](Field<Type>& self, const Field<Type>& f) -> Field<Type>& {
        self -= f;
        return self;
    }, nb::rv_policy::reference_internal)
    .def("__isub__", [](Field<Type>& self, const tmp<Field<Type>>& f) -> Field<Type>& {
        self -= f();
        return self;
    }, nb::rv_policy::reference_internal)
    .def("__isub__", [](Field<Type>& self, const Type& s) -> Field<Type>& {
        self -= s;
        return self;
    }, nb::rv_policy::reference_internal)
    .def("__mul__", [](Field<Type>& self, const scalar& s) {return self * s;})
    .def("__mul__", [](Foam::Field<Type>& self, const Field<scalar>& sf)
    {
        return self * sf;
    })
    .def("__mul__", [](Foam::Field<Type>& self, const tmp<Field<scalar>>& sf)
    {
        return self * sf();
    })
    .def("__truediv__", [](Field<Type>& self, const scalar& s) {return self / s;})
    .def("__truediv__", [](Field<Type>& self, const Field<scalar>& sf)
    {
        return self / sf;
    })
    .def("__truediv__", [](Field<Type>& self, const tmp<Field<scalar>>& sf)
    {
        return self / sf();
    })
    .def("__array__", [](Field<Type>& self, nb::object /*dtype*/ = nb::none(), nb::object /*copy*/ = nb::none()) {
        if constexpr (std::is_same<Type, Foam::scalar>::value) {
            size_t shape[1] = {(size_t)self.size()};
            return nb::ndarray<nb::numpy, Foam::scalar>(self.data(), 1, shape);
        } else {
            constexpr int nComps = Foam::pTraits<Type>::nComponents;
            size_t shape[2] = {(size_t)self.size(), (size_t)nComps};
            return nb::ndarray<nb::numpy, Foam::scalar>((Foam::scalar*)self.data(), 2, shape);
        }
    }, nb::arg("dtype") = nb::none(), nb::arg("copy") = nb::none(), nb::rv_policy::reference_internal)
    ;

    // List-of-sequences constructor for compound types (vector, tensor, symmTensor)
    if constexpr (!std::is_same_v<Type, Foam::scalar>) {
        fieldClass.def("__init__", [](Field<Type>* self, nb::list outer) {
            constexpr int nComps = Foam::pTraits<Type>::nComponents;
            new (self) Field<Type>(outer.size());
            for (size_t i = 0; i < outer.size(); ++i) {
                nb::sequence inner = nb::cast<nb::sequence>(outer[i]);
                if (nb::len(inner) != nComps)
                    throw std::runtime_error(
                        "Each inner sequence must have "
                        + std::to_string(nComps) + " components");
                for (int j = 0; j < nComps; ++j)
                    (*self)[i][j] = nb::cast<Foam::scalar>(inner[j]);
            }
        });
    }

    return fieldClass;
}

template<class Type>
nb::class_<tmp<Field<Type>>> declare_tmp_fields(nb::module_ &m, std::string className) {
    std::string tmp_className = "tmp_" + className;

    auto tmpFieldClass = nb::class_<tmp<Field<Type>>>(m, tmp_className.c_str())
    .def("__call__",[](tmp<Field<Type>>& self) -> Field<Type>&
    {
        return self.ref();
    }, nb::rv_policy::reference_internal)
    .def("__neg__", [](const tmp<Field<Type>>& self) {
        return -self();
    })
    .def("__len__", [](const tmp<Field<Type>>& self) {
        return self().size();
    })
    .def("__getitem__", [](const tmp<Field<Type>>& self, const label idx) {
        if (idx >= self().size())
        {
            throw nb::index_error();
        }
        return self()[idx];
    })
    .def("__add__", [](const tmp<Field<Type>>& self, const Field<Type>& f) {
        return self() + f;
    })
    .def("__add__", [](const tmp<Field<Type>>& self, const tmp<Field<Type>>& f) {
        return self() + f();
    })
    .def("__add__", [](const tmp<Field<Type>>& self, const Type& s) {
        return self() + s;
    })
    .def("__sub__", [](const tmp<Field<Type>>& self, const Field<Type>& f) {
        return self() - f;
    })
    .def("__sub__", [](const tmp<Field<Type>>& self, const tmp<Field<Type>>& f) {
        return self() - f();
    })
    .def("__sub__", [](const tmp<Field<Type>>& self, const Type& s) {
        return self() - s;
    })
    .def("__mul__", [](const tmp<Field<Type>>& self, const scalar& s) {
        return self() * s;
    })
    .def("__rmul__", [](const tmp<Field<Type>>& self, const scalar& s) {
        return s * self();
    })
    .def("__mul__", [](const tmp<Field<Type>>& self, const Field<scalar>& sf) {
        return self() * sf;
    })
    .def("__mul__", [](const tmp<Field<Type>>& self, const tmp<Field<scalar>>& sf) {
        return self() * sf();
    })
    .def("__truediv__", [](const tmp<Field<Type>>& self, const scalar& s) {
        return self() / s;
    })
    .def("__truediv__", [](const tmp<Field<Type>>& self, const Field<scalar>& sf) {
        return self() / sf;
    })
    .def("__truediv__", [](const tmp<Field<Type>>& self, const tmp<Field<scalar>>& sf) {
        return self() / sf();
    })
    ;

    return tmpFieldClass;
}

}

void Foam::bindFields(nb::module_& m)
{
    nb::class_<instantList>(m, "instantList")
        .def("__getitem__", [](const instantList& self, const label idx) {
            if (idx >= self.size())
            {
                throw nb::index_error();
            }
            return self[idx];
        })
    ;

    nb::class_<faceList>(m, "faceList")
        .def("__init__", [](faceList* self, const faceList& other) {
            new (self) faceList(other);
        })
        .def("__init__", [](faceList* self, const std::vector<std::vector<Foam::label>>& faces) {
            new (self) Foam::faceList(faces.size());
            for (size_t i = 0; i < faces.size(); ++i) {
                (*self)[i] = Foam::face(faces[i].size());
                for (size_t j = 0; j < faces[i].size(); ++j) {
                    (*self)[i][j] = faces[i][j];
                }
            }
        }, nb::arg("faces"));


    nb::class_<List<bool>>(m, "boolList")
        .def("__init__", [](List<bool>* self, const List<bool>& other) {
            new (self) List<bool>(other);
        })
        .def("__init__", [](List<bool>* self, std::vector<bool> vec) {
            new (self) List<bool>(vec.size());
            forAll(*self, i)
            {
                (*self)[i] = vec[i];
            }
        }, nb::arg("vec"))
        .def("__len__", [](const List<bool>& self) {
            return self.size();
        })
        .def("__getitem__", [](const List<bool>& self, const label idx) {
            if (idx >= self.size())
            {
                throw nb::index_error();
            }
            return self[idx];
        })
        .def("__setitem__", [](List<bool>& self, const label idx,const bool& s) {
            self[idx] = s;
        })
        .def("list",[](List<bool>& self){
            std::vector<bool> l_out(self.size());
            forAll(self,i)
            {
                l_out[i] = self[i];
            }
            return l_out;
        })
        ;

    nb::class_<List<label>>(m, "labelList")
        .def(nb::init<label, label > ())
        .def("__init__", [](List<label>* self, const List<label>& other) {
            new (self) List<label>(other);
        })
        .def("__init__", [](List<label>* self, std::vector<label> vec) {
            new (self) List<label>(vec.size());
            forAll(*self, i) { (*self)[i] = vec[i]; }
        }, nb::arg("vec"))
        .def("__len__", [](const List<label>& self) {
            return self.size();
        })
        .def("__getitem__", [](const List<label>& self, const label idx) {
            if (idx >= self.size())
            {
                throw nb::index_error();
            }
            return self[idx];
        })
        .def("__setitem__", [](List<label>& self, const label idx,const label& s) {
            self[idx] = s;
        })
        .def("list",[](List<label>& self){
            std::vector<label> l_out(self.size());
            forAll(self,i)
            {
                l_out[i] = self[i];
            }
            return l_out;
        })
        ;

    nb::class_<List<word>>(m, "wordList")
        .def("__init__", [](List<word>* self, const List<word>& other) {
            new (self) List<word>(other);
        })
        .def("__init__", [](List<word>* self, std::vector<std::string> vec) {
            new (self) List<word>(vec.size());
            forAll(*self, i) { (*self)[i] = word(vec[i]); }
        })
        .def("__len__", [](const List<word>& self) {
            return self.size();
        })
        .def("__getitem__", [](const List<word>& self, const label idx) {
            if (idx >= self.size())
            {
                throw nb::index_error();
            }
            return std::string(self[idx]);
        })
        .def("__setitem__", [](List<word>& self, const label idx,const std::string& s) {
            self[idx] = s;
        })
        .def("list",[](List<word>& self){
            std::vector<std::string> l_out(self.size());
            forAll(self,i)
            {
                l_out[i] = self[i];
            }
            return l_out;
        })
        ;


    auto sf = declare_fields<scalar>(m, std::string("scalarField"));
    auto tmp_sf = declare_tmp_fields<scalar>(m, std::string("scalarField"));

    auto vf = declare_fields<vector>(m, std::string("vectorField"))
    .def("__and__", [](Field<vector>& self, const vector& s) {return self & s;})
    .def("__and__", [](Field<vector>& self, const Field<vector>& sf)
    {
        return self & sf;
    })
    .def("__and__", [](Field<vector>& self, const tensor& s) {return self & s;})
    .def("__and__", [](Field<vector>& self, const Field<tensor>& sf)
    {
        return self & sf;
    })
    .def("__and__", [](Field<vector>& self, const symmTensor& s) {return self & s;})
    .def("__and__", [](Field<vector>& self, const Field<symmTensor>& sf)
    {
        return self & sf;
    })
    ;
    auto tmp_vf = declare_tmp_fields<vector>(m, std::string("vectorField"))
    .def("__and__", [](const tmp<Field<vector>>& self, const vector& s) {
        return self() & s;
    })
    .def("__and__", [](const tmp<Field<vector>>& self, const Field<vector>& sf) {
        return self() & sf;
    })
    .def("__and__", [](const tmp<Field<vector>>& self, const tmp<Field<vector>>& sf) {
        return self() & sf();
    })
    ;


    auto tf = declare_fields<tensor>(m, std::string("tensorField"))
    // .def("__iand__", [](Field<tensor>& self, const vector& s) {return Field<vector>(self & s);})
    // .def("__iand__", [](Field<tensor>& self, const Field<vector>& sf)
    // {
    //     return Field<vector>(self & sf);
    // })
    ;
    auto tmp_tf = declare_tmp_fields<tensor>(m, std::string("tensorField"));


    auto stf = declare_fields<symmTensor>(m, std::string("symmTensorField"))
    // .def("__iand__", [](Field<symmTensor>& self, const vector& s) {return Field<vector>(self & s);})
    // .def("__iand__", [](Field<symmTensor>& self, const Field<vector>& sf)
    // {
    //     return Field<vector>(self & sf);
    // })
    ;
    auto tmp_stf = declare_tmp_fields<symmTensor>(m, std::string("symmTensorField"));





    m.def("sum",declare_sum<scalar>);
    m.def("sum",declare_sum<vector>);
    m.def("sum",declare_sum<tensor>);
    m.def("sum",declare_sum<symmTensor>);

    // ==== uniformDimensionedVectorField bindings ====
    // Used for reading constant fields like gravity
    nb::class_<Foam::uniformDimensionedVectorField>(m, "uniformDimensionedVectorField")
        .def("__init__", [](Foam::uniformDimensionedVectorField* self, const Foam::fvMesh& mesh, const std::string& fieldName) {
            new (self) Foam::uniformDimensionedVectorField(
                Foam::IOobject(
                    fieldName,
                    mesh.time().constant(),
                    mesh,
                    Foam::IOobject::MUST_READ,
                    Foam::IOobject::NO_WRITE
                )
            );
        }, nb::arg("mesh"),
            nb::arg("fieldName"),
            "Read a uniformDimensionedVectorField from constant/ directory")
        .def("value", [](const Foam::uniformDimensionedVectorField& self) {
            return self.value();
        }, "Get the uniform vector value")
        .def("name", [](const Foam::uniformDimensionedVectorField& self) {
            return self.name();
        }, "Get the field name")
        .def("dimensions", [](const Foam::uniformDimensionedVectorField& self) {
            return self.dimensions();
        }, "Get the field dimensions")
        .def("__and__", [](const Foam::uniformDimensionedVectorField& self,
                           const Foam::volVectorField& vf) {
            // Use the base class dimensioned<vector> which has operator& defined
            const Foam::dimensioned<Foam::vector>& dv = self;
            return dv & vf;
        }, nb::arg("vf"), "Dot product with volVectorField, returns tmp<volScalarField>")
        .def("__and__", [](const Foam::uniformDimensionedVectorField& self,
                           const Foam::surfaceVectorField& vf) {
            // Use the base class dimensioned<vector> which has operator& defined
            const Foam::dimensioned<Foam::vector>& dv = self;
            return dv & vf;
        }, nb::arg("vf"), "Dot product with surfaceVectorField, returns tmp<surfaceScalarField>")
        ;

    // ==== uniformDimensionedScalarField bindings ====
    nb::class_<Foam::uniformDimensionedScalarField>(m, "uniformDimensionedScalarField")
        .def("__init__", [](Foam::uniformDimensionedScalarField* self, const Foam::fvMesh& mesh, const std::string& fieldName) {
            new (self) Foam::uniformDimensionedScalarField(
                Foam::IOobject(
                    fieldName,
                    mesh.time().constant(),
                    mesh,
                    Foam::IOobject::MUST_READ,
                    Foam::IOobject::NO_WRITE
                )
            );
        }, nb::arg("mesh"),
            nb::arg("fieldName"),
            "Read a uniformDimensionedScalarField from constant/ directory")
        .def("value", [](const Foam::uniformDimensionedScalarField& self) {
            return self.value();
        }, "Get the uniform scalar value")
        .def("name", [](const Foam::uniformDimensionedScalarField& self) {
            return self.name();
        }, "Get the field name")
        .def("dimensions", [](const Foam::uniformDimensionedScalarField& self) {
            return self.dimensions();
        }, "Get the field dimensions")
        ;
}
