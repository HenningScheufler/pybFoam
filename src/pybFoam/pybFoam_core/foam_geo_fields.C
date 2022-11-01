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

#include "foam_geo_fields.H"



namespace Foam
{

// namespace py = pybind11;

template<class Type, template<class> class PatchField, class GeoMesh>
const Field<Type>& field(const GeometricField<Type, PatchField, GeoMesh>& vf,const fvMesh& mesh, const std::string& name)
{
    if (name == "internalField")
    {
        return vf.primitiveField();
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
        return vf.boundaryField()[patchId];
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
py::class_< GeometricField<Type, PatchField, GeoMesh> > declare_geofields(py::module &m, std::string className) {
    auto geofieldClass = py::class_< Foam::GeometricField<Type, PatchField, GeoMesh>>(m, className.c_str())
    .def(py::init<GeometricField<Type, PatchField, GeoMesh>>())
    // .def(py::init([]
    // (
    //     const Foam::fvMesh& mesh,
    //     const std::string& name
    // )
    // {
    //     return Foam::GeometricField<Type, PatchField, GeoMesh>
    //     (
    //         IOobject
    //         (
    //             name,
    //             mesh.time().timeName(),
    //             mesh,
    //             Foam::IOobject::MUST_READ,
    //             Foam::IOobject::AUTO_WRITE
    //         ),
    //         mesh
    //     );
    // }))
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
    .def("internalField", [](
        const Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const std::string& name)
    {
        return self.primitiveField();
    })
    .def("__getitem__", []
    (
        const Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const std::string& name
    )
    {
        return Foam::field(self,self.mesh(),name);
    })
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
        return Foam::GeometricField<Type, PatchField, GeoMesh>(self + vf2);
    })
    .def("__sub__", []
    (
        Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const Foam::GeometricField<Type, PatchField, GeoMesh>& vf2
    )
    {
        return Foam::GeometricField<Type, PatchField, GeoMesh>(self - vf2);
    })
    .def("__mul__", []
    (
        Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const Foam::GeometricField<scalar, PatchField, GeoMesh>& vf2)
    {
        return Foam::GeometricField<Type, PatchField, GeoMesh>(self * vf2);
    })
    .def("__truediv__", []
    (
        Foam::GeometricField<Type, PatchField, GeoMesh>& self,
        const Foam::GeometricField<scalar, PatchField, GeoMesh>& vf2
    )
    {
        return Foam::GeometricField<Type, PatchField, GeoMesh>(self / vf2);
    })

    ;
    return geofieldClass;
}


template<class Type, template<class> class PatchField, class GeoMesh>
GeometricField<scalar, PatchField, GeoMesh>
declare_mag (const GeometricField<Type, PatchField, GeoMesh>& geof)
{
    return GeometricField<scalar, PatchField, GeoMesh>(mag(geof));
}


}


void AddPyGeoFields(py::module& m)
{
    namespace py = pybind11;

    auto vsf = Foam::declare_geofields<Foam::scalar,Foam::fvPatchField, Foam::volMesh>(m, std::string("volScalarField"));
    auto vvf = Foam::declare_geofields<Foam::vector,Foam::fvPatchField, Foam::volMesh>(m, std::string("volVectorField"));
    auto vtf = Foam::declare_geofields<Foam::tensor,Foam::fvPatchField, Foam::volMesh>(m, std::string("volTensorField"));
    auto vstf = Foam::declare_geofields<Foam::symmTensor,Foam::fvPatchField, Foam::volMesh>(m, std::string("volSymmTensorField"));

    auto ssf = Foam::declare_geofields<Foam::scalar,Foam::fvsPatchField, Foam::surfaceMesh>(m, std::string("surfaceScalarField"));
    auto svf = Foam::declare_geofields<Foam::vector,Foam::fvsPatchField, Foam::surfaceMesh>(m, std::string("surfaceVectorField"));
    auto stf = Foam::declare_geofields<Foam::tensor,Foam::fvsPatchField, Foam::surfaceMesh>(m, std::string("surfaceTensorField"));

    // // functions

    m.def("mag",Foam::declare_mag<Foam::scalar, Foam::fvPatchField, Foam::volMesh>);
    m.def("mag",Foam::declare_mag<Foam::vector, Foam::fvPatchField, Foam::volMesh>);
    m.def("mag",Foam::declare_mag<Foam::tensor, Foam::fvPatchField, Foam::volMesh>);
    m.def("mag",Foam::declare_mag<Foam::symmTensor, Foam::fvPatchField, Foam::volMesh>);

    m.def("mag",Foam::declare_mag<Foam::scalar, Foam::fvsPatchField, Foam::surfaceMesh>);
    m.def("mag",Foam::declare_mag<Foam::vector, Foam::fvsPatchField, Foam::surfaceMesh>);
    m.def("mag",Foam::declare_mag<Foam::tensor, Foam::fvsPatchField, Foam::surfaceMesh>);

}