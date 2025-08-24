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

#include "bind_geo_fields.hpp"
#include "tmp.H"


namespace Foam
{

// namespace py = pybind11;

template<class Type, template<class> class PatchField, class GeoMesh>
Field<Type>& field(GeometricField<Type, PatchField, GeoMesh>& vf,const fvMesh& mesh, const std::string& name)
{
    if (name == "internalField")
    {
        return vf.primitiveFieldRef();
    }
    else
    {
        label patchId = mesh.boundaryMesh().findPatchID(name);
        if (patchId == -1)
        {
            FatalErrorInFunction
                << "patch not found " << nl
                << exit(FatalError);
        }
        return vf.boundaryFieldRef()[patchId];
    }
}

template<class Type, template<class> class PatchField, class GeoMesh>
void field(GeometricField<Type, PatchField, GeoMesh>& vf,const fvMesh& mesh, const std::string& name,const Field<Type>& f)
{
    if (name == "internalField")
    {
        vf.primitiveFieldRef() = f;
    }
    else
    {
        label patchId = mesh.boundaryMesh().findPatchID(name);
        if (patchId == -1)
        {
            FatalErrorInFunction
                << "patch not found " << nl
                << exit(FatalError);
        }
        vf.boundaryFieldRef()[patchId] = f;
    }
}

template<class Type, template<class> class PatchField, class GeoMesh>
auto declare_geofields(py::module &m, std::string className) {
    std::string tmp_className = "tmp_" + className;
    auto tmpGeofieldClass = py::class_< tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>>(m, tmp_className.c_str())
    .def("__call__",[](const tmp<Foam::GeometricField<Type, PatchField, GeoMesh>>& self)
    {
        return Foam::GeometricField<Type, PatchField, GeoMesh>(self);
    }, py::return_value_policy::reference_internal
    )
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
    ;

    auto geofieldClass = py::class_< Foam::GeometricField<Type, PatchField, GeoMesh>>(m, className.c_str())
    .def(py::init<GeometricField<Type, PatchField, GeoMesh>>())
    .def(py::init<tmp<GeometricField<Type, PatchField, GeoMesh>>>())
    .def(py::init<const word &,tmp<GeometricField<Type, PatchField, GeoMesh>>>())
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
    },py::return_value_policy::reference)
    .def_static("from_registry",[](const fvMesh& mesh,std::string name)
    {
        const Foam::GeometricField<Type, PatchField, GeoMesh>* obj =
            mesh.findObject<Foam::GeometricField<Type, PatchField, GeoMesh>>(name);
        return obj;
    },py::return_value_policy::reference)
    .def_static("list_objects",[](const fvMesh& mesh)
    {
        return mesh.names<Foam::GeometricField<Type, PatchField, GeoMesh>>();
    })
    // .def("internalField",&Foam::GeometricField<Type, PatchField, GeoMesh>::primitiveFieldRef, py::return_value_policy::reference_internal)
    .def("internalField", [](
        Foam::GeometricField<Type, PatchField, GeoMesh>& self
    ) -> Foam::Field<Type>&
    {
        return self.primitiveFieldRef();
    }, py::return_value_policy::reference_internal)
    .def("__getitem__", []
    (
        Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const std::string& name
    ) -> Foam::Field<Type>&
    {
        return Foam::field(self,self.mesh(),name);
    }, py::return_value_policy::reference_internal)
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

    ;

    m.def("write", [](const Foam::GeometricField<Type, PatchField, GeoMesh>& geofield)
    {
        geofield.write();
    });
    return std::make_tuple(geofieldClass, tmpGeofieldClass);
}


template<class Type, template<class> class PatchField, class GeoMesh>
GeometricField<scalar, PatchField, GeoMesh>
declare_mag (const GeometricField<Type, PatchField, GeoMesh>& geof)
{
    return mag(geof);
}


}


void Foam::bindGeoFields(py::module& m)
{
    namespace py = pybind11;

    auto [vsf, tmp_vsf] = declare_geofields<scalar,fvPatchField, volMesh>(m, std::string("volScalarField"));
    auto [vvf, tmp_vvf] = declare_geofields<vector,fvPatchField, volMesh>(m, std::string("volVectorField"));
    auto [vtf, tmp_vtf] = declare_geofields<tensor,fvPatchField, volMesh>(m, std::string("volTensorField"));
    auto [vstf, tmp_vstf] = declare_geofields<symmTensor,fvPatchField, volMesh>(m, std::string("volSymmTensorField"));
    tmp_vsf.def("__truediv__", [](const tmp<Foam::GeometricField<scalar, Foam::fvPatchField, Foam::volMesh>>& self, const scalar& rhs)
    {
        return self / rhs;
    })
    .def("__rtruediv__", [](const tmp<volScalarField>& self, const scalar& lhs)
    {
        return lhs / self;
    })
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
    ;

    vsf.def("__mul__", [](const volScalarField& self, const volVectorField& lhs)
    {
        return self * lhs;
    })
    .def("__mul__", [](const volScalarField& self, const tmp<volVectorField>& lhs)
    {
        return self * lhs;
    });

    auto [ssf, tmp_ssf] = declare_geofields<scalar,fvsPatchField, surfaceMesh>(m, std::string("surfaceScalarField"));
    auto [svf, tmp_svf] = declare_geofields<vector,fvsPatchField, surfaceMesh>(m, std::string("surfaceVectorField"));
    auto [stf, tmp_stf] = declare_geofields<tensor,fvsPatchField, surfaceMesh>(m, std::string("surfaceTensorField"));

    // // functions

    m.def("mag",declare_mag<scalar, fvPatchField, volMesh>);
    m.def("mag",declare_mag<vector, fvPatchField, volMesh>);
    m.def("mag",declare_mag<tensor, fvPatchField, volMesh>);
    m.def("mag",declare_mag<symmTensor, fvPatchField, volMesh>);

    m.def("mag",declare_mag<scalar, fvsPatchField, surfaceMesh>);
    m.def("mag",declare_mag<vector, fvsPatchField, surfaceMesh>);
    m.def("mag",declare_mag<tensor, fvsPatchField, surfaceMesh>);

}
