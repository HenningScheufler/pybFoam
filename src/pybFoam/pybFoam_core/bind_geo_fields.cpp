/*---------------------------------------------------------------------------*\
            Copyright (c) 2022-2026, Henning Scheufler
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

#include "bind_geo_fields.hpp"
#include "tmp.H"
#include "bound.H"

namespace Foam
{

template<class Type, template<class> class PatchField, class GeoMesh>
Field<Type>& field(GeometricField<Type, PatchField, GeoMesh>& gf, const std::string& name)
{
    if (name == "internalField")
    {
        return gf.primitiveFieldRef();
    }
    else
    {
        label patchId = gf.mesh().boundaryMesh().findPatchID(name);
        if (patchId == -1)
        {
            FatalErrorInFunction
                << "patch not found " << nl
                << exit(FatalError);
        }
        return gf.boundaryFieldRef()[patchId];
    }
}

template<class Type, template<class> class PatchField, class GeoMesh>
void field(GeometricField<Type, PatchField, GeoMesh>& gf, const std::string& name, const Field<Type>& f)
{
    if (name == "internalField")
    {
        gf.primitiveFieldRef() = f;
    }
    else
    {
        label patchId = gf.mesh().boundaryMesh().findPatchID(name);
        if (patchId == -1)
        {
            FatalErrorInFunction
                << "patch not found " << nl
                << exit(FatalError);
        }
        gf.boundaryFieldRef()[patchId] = f;
    }
}

template<class Type, template<class> class PatchField, class GeoMesh>
auto declare_geofields(nb::module_ &m, std::string className) {
    std::string tmp_className = "tmp_" + className;
    auto tmpGeofieldClass = nb::class_< tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>>(m, tmp_className.c_str())
    .def("__call__",[](tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& self) -> Foam::GeometricField<Type, PatchField, GeoMesh>&
    {
        return self.ref();
    }, nb::rv_policy::reference_internal)
    .def("__neg__", [](const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& self) {
        return -self;
    })
    .def("__add__", [](const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& self,
                       const Foam::GeometricField<Type, PatchField, GeoMesh>& vf2)
    {
        return self + vf2;
    })
    .def("__add__", [](const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& self,
                       const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& vf2)
    {
        return self + vf2;
    })
    .def("__sub__", [](const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& self,
                       const Foam::GeometricField<Type, PatchField, GeoMesh>& vf2)
    {
        return self - vf2;
    })
    .def("__sub__", [](const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& self,
                       const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& vf2)
    {
        return self - vf2;
    })
    .def("__mul__", []
    (
        const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& self,
        const Foam::GeometricField<scalar, PatchField, GeoMesh>& vf2)
    {
        return self * vf2;
    })
    .def("__mul__", []
    (
        const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& self,
        const tmp<Foam::GeometricField<scalar, PatchField, GeoMesh>>& vf2)
    {
        return self * vf2;
    })
    .def("__mul__", []
    (
        const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& self,
        const scalar& s
    )
    {
        return self * s;
    })
    .def("__rmul__", []
    (
        const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& self,
        const scalar& s
    )
    {
        return s * self;
    })
    // dimensioned operators (tmp op dimensioned)
    .def("__mul__", [](const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& self, const dimensioned<scalar>& ds)
    {
        return self * ds;
    })
    .def("__truediv__", [](const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& self, const dimensioned<scalar>& ds)
    {
        return self / ds;
    })
    .def("__add__", [](const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& self, const dimensioned<Type>& ds)
    {
        return self + ds;
    })
    .def("__sub__", [](const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& self, const dimensioned<Type>& ds)
    {
        return self - ds;
    })
    ;

    auto geofieldClass = nb::class_< Foam::GeometricField<Type, PatchField, GeoMesh>>(m, className.c_str())
    .def(nb::init<GeometricField<Type, PatchField, GeoMesh>>())
    .def(nb::init<tmp<GeometricField<Type, PatchField, GeoMesh>>>())
    .def(nb::init<const word &,tmp<GeometricField<Type, PatchField, GeoMesh>>>())
    .def("correctBoundaryConditions", &Foam::GeometricField<Type, PatchField, GeoMesh>::correctBoundaryConditions)
    .def_static("read_field",[](const fvMesh& mesh,std::string name)
    {
        Foam::GeometricField<Type, PatchField, GeoMesh>* geoField
        (
            new Foam::GeometricField<Type, PatchField, GeoMesh>
            (
                Foam::IOobject
                (
                    name,
                    mesh.time().timeName(),
                    mesh,
                    Foam::IOobject::MUST_READ,
                    Foam::IOobject::AUTO_WRITE
                ),
                mesh
            )
        );
        mesh.objectRegistry::store(geoField);
        return geoField;
    },nb::rv_policy::reference)
    .def_static("from_registry",[](const fvMesh& mesh,std::string name)
    {
        const Foam::GeometricField<Type, PatchField, GeoMesh>* obj =
            mesh.findObject<Foam::GeometricField<Type, PatchField, GeoMesh>>(name);
        return obj;
    },nb::rv_policy::reference)
    .def_static("list_objects",[](const fvMesh& mesh)
    {
        return mesh.names<Foam::GeometricField<Type, PatchField, GeoMesh>>();
    })
    // .def("internalField",&Foam::GeometricField<Type, PatchField, GeoMesh>::primitiveFieldRef, nb::rv_policy::reference_internal)
    .def("internalField", [](
        Foam::GeometricField<Type, PatchField, GeoMesh>& self
    ) -> Foam::Field<Type>&
    {
        return self.primitiveFieldRef();
    }, nb::rv_policy::reference_internal)
    .def("__getitem__", []
    (
        Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const std::string& name
    ) -> Foam::Field<Type>&
    {
        return Foam::field(self,name);
    }, nb::rv_policy::reference_internal)
    .def("__setitem__", []
    (
        Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const std::string& name,
        const Foam::Field<Type>& f
    )
    {
        Foam::field(self,self.mesh(),name,f);
    })
    .def("__add__", []
    (
        Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const Foam::GeometricField<Type, PatchField, GeoMesh>& vf2
    )
    {
        return self + vf2;
    })
    .def("__add__", [](const Foam::GeometricField<Type, PatchField, GeoMesh>& self,
                       const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& vf2)
    {
        return self + vf2;
    })
    .def("__sub__", []
    (
        Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const Foam::GeometricField<Type, PatchField, GeoMesh>& vf2
    )
    {
        return self - vf2;
    })
    .def("__sub__", [](const Foam::GeometricField<Type, PatchField, GeoMesh>& self,
                       const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& vf2)
    {
        return self - vf2;
    })
    .def("__mul__", []
    (
        Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const Foam::GeometricField<scalar, PatchField, GeoMesh>& vf2)
    {
        return self * vf2;
    })
    .def("__mul__", []
    (
        Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const tmp<Foam::GeometricField<scalar, PatchField, GeoMesh>>& vf2)
    {
        return self * vf2;
    })
    .def("__truediv__", []
    (
        Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const Foam::GeometricField<scalar, PatchField, GeoMesh>& vf2
    )
    {
        return self / vf2;
    })
    .def("__truediv__", []
    (
        const Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const scalar& s
    )
    {
        return self / s;
    })
    .def("__truediv__", []
    (
        const Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const tmp<Foam::GeometricField<scalar, PatchField, GeoMesh>>& rhs
    )
    {
        return self / rhs;
    })
    .def("__mul__", []
    (
        const Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const scalar& s
    )
    {
        return self * s;
    })
    .def("__rmul__", []
    (
        const Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const scalar& s
    )
    {
        return s * self;
    })
    // dimensioned operators (field op dimensioned)
    .def("__mul__", [](const Foam::GeometricField<Type, PatchField, GeoMesh>& self, const dimensioned<scalar>& ds)
    {
        return self * ds;
    })
    .def("__truediv__", [](const Foam::GeometricField<Type, PatchField, GeoMesh>& self, const dimensioned<scalar>& ds)
    {
        return self / ds;
    })
    .def("__add__", [](const Foam::GeometricField<Type, PatchField, GeoMesh>& self, const dimensioned<Type>& ds)
    {
        return self + ds;
    })
    .def("__sub__", [](const Foam::GeometricField<Type, PatchField, GeoMesh>& self, const dimensioned<Type>& ds)
    {
        return self - ds;
    })
    .def("__neg__", []
    (
        const Foam::GeometricField<Type, PatchField, GeoMesh>& self
    )
    {
        return -self;
    })
    .def("select", &Foam::GeometricField<Type, PatchField, GeoMesh>::select)
    .def("assign", []
    (
        Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const Foam::GeometricField<Type, PatchField, GeoMesh>& vf2
    )
    {
        self = vf2;
    })
    .def("assign", []
    (
        Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& vf2
    )
    {
        self = vf2;
    })
    .def("relax", []
    (
        Foam::GeometricField<Type, PatchField, GeoMesh>& self
    )
    {
        self.relax();
    })
    .def("relax", []
    (
        Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        Foam::scalar relaxFactor
    )
    {
        self.relax(relaxFactor);
    })
    .def("mesh", []
    (
        const Foam::GeometricField<Type, PatchField, GeoMesh>& self
    ) -> const typename GeoMesh::Mesh&
    {
        return self.mesh();
    }, nb::rv_policy::reference)

    ;

    m.def("write", [](const Foam::GeometricField<Type, PatchField, GeoMesh>& geofield)
    {
        geofield.write();
    });
    return std::make_tuple(geofieldClass, tmpGeofieldClass);
}

}  // End namespace Foam


void Foam::bindGeoFields(nb::module_& m)
{

    auto [vsf, tmp_vsf] = declare_geofields<scalar,fvPatchField, volMesh>(m, std::string("volScalarField"));
    auto [vvf, tmp_vvf] = declare_geofields<vector,fvPatchField, volMesh>(m, std::string("volVectorField"));
    auto [vtf, tmp_vtf] = declare_geofields<tensor,fvPatchField, volMesh>(m, std::string("volTensorField"));
    auto [vstf, tmp_vstf] = declare_geofields<symmTensor,fvPatchField, volMesh>(m, std::string("volSymmTensorField"));
    tmp_vsf.def("__truediv__", [](const tmp<Foam::GeometricField<scalar, Foam::fvPatchField, Foam::volMesh>>& self, const scalar& rhs)
    {
        return self / rhs;
    })
    .def("__truediv__", [](const tmp<volScalarField>& self, const volScalarField& rhs)
    {
        return self / rhs;
    })
    .def("__truediv__", [](const tmp<volScalarField>& self, const tmp<volScalarField>& rhs)
    {
        return self / rhs;
    })
    .def("__rtruediv__", [](const tmp<volScalarField>& self, const scalar& lhs)
    {
        return lhs / self;
    })
    .def("__add__", [](const tmp<volScalarField>& self, const scalar& rhs)
    {
        return self + dimensionedScalar("s", self().dimensions(), rhs);
    })
    // .def("__radd__", [](const tmp<volScalarField>& self, const scalar& lhs)
    // {
    //     return dimensionedScalar("s", self().dimensions(), lhs) + self;
    // })
    .def("__sub__", [](const tmp<volScalarField>& self, const scalar& rhs)
    {
        return self - dimensionedScalar("s", self().dimensions(), rhs);
    })
    // .def("__rsub__", [](const tmp<volScalarField>& self, const scalar& lhs)
    // {
    //     return dimensionedScalar("s", self().dimensions(), lhs) - self;
    // })
    .def("__mul__", []
    (
        const tmp<volScalarField>& self,
        const volVectorField& lhs
    )
    {
        return self * lhs;
    })
    .def("__mul__", []
    (
        const tmp<volScalarField>& self,
        const tmp<volVectorField>& lhs
    )
    {
        return self * lhs;
    })
    .def("__mul__", []
    (
        const tmp<volScalarField>& self,
        const volTensorField& lhs
    )
    {
        return self * lhs;
    })
    .def("__mul__", []
    (
        const tmp<volScalarField>& self,
        const tmp<volTensorField>& lhs
    )
    {
        return self * lhs;
    })
    // Scalar arithmetic operators for tmp<volScalarField> (needed for Boussinesq)
    // .def("__rsub__", [](const tmp<volScalarField>& self, const scalar& s)
    // {
    //     return s - self;
    // })
    // .def("__radd__", [](const tmp<volScalarField>& self, const scalar& s)
    // {
    //     return s + self;
    // })
    ;

    vsf.def("__mul__", [](const volScalarField& self, const volVectorField& lhs)
    {
        return self * lhs;
    })
    .def("__mul__", [](const volScalarField& self, const tmp<volVectorField>& lhs)
    {
        return self * lhs;
    })
    .def("__mul__", [](const volScalarField& self, const volTensorField& lhs)
    {
        return self * lhs;
    })
    .def("__mul__", [](const volScalarField& self, const tmp<volTensorField>& lhs)
    {
        return self * lhs;
    })
    // Scalar arithmetic operators for volScalarField (needed for Boussinesq: 1.0 - beta*(T - TRef))
    .def("__sub__", [](const volScalarField& self, const scalar& s)
    {
        return self - dimensionedScalar("s", self.dimensions(), s);
    })
    // .def("__rsub__", [](const volScalarField& self, const scalar& s)
    // {
    //     return s - self;
    // })
    .def("__add__", [](const volScalarField& self, const scalar& s)
    {
        return self + dimensionedScalar("s", self.dimensions(), s);
    })
    // .def("__radd__", [](const volScalarField& self, const scalar& s)
    // {
    //     return s + self;
    // })
    .def("__rtruediv__", [](const volScalarField& self, const scalar& s)
    {
        return s / self;
    });

    auto [ssf, tmp_ssf] = declare_geofields<scalar,fvsPatchField, surfaceMesh>(m, std::string("surfaceScalarField"));

    // Add division operators for tmp_surfaceScalarField (needed for Boussinesq)
    tmp_ssf.def("__truediv__", [](const tmp<Foam::GeometricField<scalar, Foam::fvsPatchField, Foam::surfaceMesh>>& self, const scalar& rhs)
    {
        return self / rhs;
    })
    .def("__truediv__", [](const tmp<surfaceScalarField>& self, const surfaceScalarField& rhs)
    {
        return self / rhs;
    })
    .def("__truediv__", [](const tmp<surfaceScalarField>& self, const tmp<surfaceScalarField>& rhs)
    {
        return self / rhs;
    });

    auto [svf, tmp_svf] = declare_geofields<vector,fvsPatchField, surfaceMesh>(m, std::string("surfaceVectorField"));
    auto [stf, tmp_stf] = declare_geofields<tensor,fvsPatchField, surfaceMesh>(m, std::string("surfaceTensorField"));
    auto [sstf, tmp_sstf] = declare_geofields<symmTensor,fvsPatchField, surfaceMesh>(m, std::string("surfaceSymmTensorField"));

    // Functions

    // mag
    m.def("mag", [](const VolumeField<scalar>& f) { return Foam::mag(f); });
    m.def("mag", [](const VolumeField<vector>& f) { return Foam::mag(f); });
    m.def("mag", [](const VolumeField<tensor>& f) { return Foam::mag(f); });
    m.def("mag", [](const VolumeField<symmTensor>& f) { return Foam::mag(f); });

    m.def("mag", [](const SurfaceField<scalar>& f) { return Foam::mag(f); });
    m.def("mag", [](const SurfaceField<vector>& f) { return Foam::mag(f); });
    m.def("mag", [](const SurfaceField<tensor>& f) { return Foam::mag(f); });

    // magSqr
    m.def("magSqr", [](const VolumeField<scalar>& f) { return Foam::magSqr(f); });
    m.def("magSqr", [](const VolumeField<vector>& f) { return Foam::magSqr(f); });
    m.def("magSqr", [](const VolumeField<tensor>& f) { return Foam::magSqr(f); });
    m.def("magSqr", [](const tmp<VolumeField<tensor>>& f) { return Foam::magSqr(f); });
    m.def("magSqr", [](const tmp<VolumeField<vector>>& f) { return Foam::magSqr(f); });

    // sqr (scalar → scalar, vector → symmTensor)
    m.def("sqr", [](const VolumeField<scalar>& f) { return Foam::sqr(f); });
    m.def("sqr", [](const tmp<VolumeField<scalar>>& f) { return Foam::sqr(f); });

    // sqrt
    m.def("sqrt", [](const VolumeField<scalar>& f) { return Foam::sqrt(f); });
    m.def("sqrt", [](const tmp<VolumeField<scalar>>& f) { return Foam::sqrt(f); });

    // pow3, pow6
    m.def("pow3", [](const volScalarField& f) { return Foam::pow3(f); });
    m.def("pow3", [](const tmp<volScalarField>& f) { return Foam::pow3(f); });
    m.def("pow6", [](const volScalarField& f) { return Foam::pow6(f); });
    m.def("pow6", [](const tmp<volScalarField>& f) { return Foam::pow6(f); });

    // skew (tensor → tensor)
    m.def("skew", [](const volTensorField& f) { return Foam::skew(f); });
    m.def("skew", [](const tmp<volTensorField>& f) { return Foam::skew(f); });

    // symm (tensor → symmTensor)
    m.def("symm", [](const volTensorField& f) { return Foam::symm(f); });
    m.def("symm", [](const tmp<volTensorField>& f) { return Foam::symm(f); });

    // T (transpose: tensor → tensor)
    m.def("T", [](const volTensorField& f) { return f.T(); });
    m.def("T", [](const tmp<volTensorField>& f) { return f().T(); });

    // max/min for field vs scalar/field
    m.def("max", [](const volScalarField& f, const dimensionedScalar& s) { return Foam::max(f, s); });
    m.def("max", [](const volScalarField& f, const volScalarField& g) { return Foam::max(f, g); });
    m.def("max", [](const volScalarField& f, const scalar& s)
    {
        return Foam::max(f, dimensionedScalar("s", f.dimensions(), s));
    });
    m.def("max", [](const tmp<volScalarField>& f, const dimensionedScalar& s) { return Foam::max(f, s); });
    m.def("max", [](const tmp<volScalarField>& f, const scalar& s)
    {
        return Foam::max(f, dimensionedScalar("s", f().dimensions(), s));
    });
    m.def("min", [](const volScalarField& f, const dimensionedScalar& s) { return Foam::min(f, s); });
    m.def("min", [](const volScalarField& f, const volScalarField& g) { return Foam::min(f, g); });
    m.def("min", [](const volScalarField& f, const scalar& s)
    {
        return Foam::min(f, dimensionedScalar("s", f.dimensions(), s));
    });
    m.def("min", [](const tmp<volScalarField>& f, const scalar& s)
    {
        return Foam::min(f, dimensionedScalar("s", f().dimensions(), s));
    });

    // pow for volScalarField
    m.def("pow", [](const volScalarField& f, const scalar& exp)
    {
        return Foam::pow(f, dimensionedScalar("exp", dimless, exp));
    });
    m.def("pow", [](const tmp<volScalarField>& f, const scalar& exp)
    {
        return Foam::pow(f, dimensionedScalar("exp", dimless, exp));
    });

    // bound
    m.def("bound", [](volScalarField& f, const dimensionedScalar& lower)
    {
        return Foam::bound(f, lower);
    });

    // devTwoSymm (tensor → symmTensor)
    m.def("devTwoSymm", [](const volTensorField& T) { return Foam::devTwoSymm(T); });
    m.def("devTwoSymm", [](const tmp<volTensorField>& T) { return Foam::devTwoSymm(T); });

    // dev2 (deviatoric: T - (2/3)*tr(T)*I for symmTensor and tensor)
    m.def("dev2", [](const volSymmTensorField& T) { return Foam::dev2(T); });
    m.def("dev2", [](const tmp<volSymmTensorField>& T) { return Foam::dev2(T); });
    m.def("dev2", [](const volTensorField& T) { return Foam::dev2(T); });
    m.def("dev2", [](const tmp<volTensorField>& T) { return Foam::dev2(T); });


    // && (double inner product: tensor && symmTensor → scalar)
    m.def("doubleInner", [](const volTensorField& T, const volSymmTensorField& S)
    {
        return T && S;
    });
    m.def("doubleInner", [](const volTensorField& T, const tmp<volSymmTensorField>& S)
    {
        return T && S;
    });

}
