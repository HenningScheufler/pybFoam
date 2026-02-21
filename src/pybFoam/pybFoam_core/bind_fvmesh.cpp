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

#include "bind_fvmesh.hpp"
#include "bind_time.hpp"
#include "bind_polymesh.hpp"
#include <nanobind/make_iterator.h>
#include "volFields.H"
#include "surfaceFields.H"
#include "dynamicFvMesh.H"
#include "fvMesh.H"
#include "polyMesh.H"
#include "fvBoundaryMesh.H"
#include "fvPatch.H"
#include "IOobject.H"

namespace Foam
{

    fvMesh *createMesh(const Time &time, bool autoWrite)
    {
        fvMesh *mesh(
            new fvMesh(
                IOobject(
                    "region0",
                    time.timeName(),
                    time,
                    IOobject::MUST_READ),
                false));
        mesh->init(true);
        if (autoWrite)
        {
            mesh->write();
        }
        return mesh;
    }

    fvMesh *createMeshFromPolyMesh(polyMesh& polyMeshRef, bool autoWrite)
    {
        // Write the polyMesh to disk
        polyMeshRef.write();

        // Get the time and location info from the polyMesh
        const Time& time = polyMeshRef.time();
        const word& regionName = polyMeshRef.name();

        // Create fvMesh by reading from disk where polyMesh was written
        fvMesh *mesh(
            new fvMesh(
                IOobject(
                    regionName,
                    time.timeName(),
                    time,
                    IOobject::MUST_READ),
                false));
        mesh->init(true);

        if (autoWrite)
        {
            mesh->write();
        }

        return mesh;
    }

}

void bindFvMesh(nanobind::module_ &m)
{
    using namespace Foam;
    namespace nb = nanobind;

    m.def("createMesh",
        [](const Foam::Time& time, bool autoWrite) {
            return Foam::createMesh(time, autoWrite);
        },
        nb::arg("time"), nb::arg("autoWrite") = false,
        nb::rv_policy::take_ownership,
        "Create a mesh from a Time object");

    nb::class_<Foam::fvBoundaryMesh>(m, "fvBoundaryMesh")
        .def("size", [](const Foam::fvBoundaryMesh& self) { return self.size(); })
        .def("__len__", [](const Foam::fvBoundaryMesh& self) { return self.size(); })
        .def("__getitem__", [](const Foam::fvBoundaryMesh& self, Foam::label i) -> const Foam::fvPatch& {
            return self[i];
        }, nb::rv_policy::reference_internal)
        .def("__iter__", [](const Foam::fvBoundaryMesh& self) {
            return nb::make_iterator(nb::type<Foam::fvBoundaryMesh>(), "iterator",
                self.begin(), self.end());
        }, nb::keep_alive<0, 1>())
        .def("findPatchID", [](const Foam::fvBoundaryMesh& self, const Foam::word& patchName) {
            return self.findPatchID(patchName);
        });

    nb::class_<Foam::fvPatch>(m, "fvPatch")
        .def("name", [](const Foam::fvPatch& self) -> const Foam::word& {
            return self.name();
        }, nb::rv_policy::reference)
        .def("size", [](const Foam::fvPatch& self) { return self.size(); })
        .def("start", [](const Foam::fvPatch& self) { return self.start(); })
        .def("index", [](const Foam::fvPatch& self) { return self.index(); });

    nb::class_<Foam::fvMesh>(m, "fvMesh")
        .def("__init__", [](Foam::fvMesh* self, const Foam::Time& time, bool autoWrite) {
             new (self) Foam::fvMesh(
                IOobject(
                    "region0",
                    time.timeName(),
                    time,
                    IOobject::MUST_READ),
                false);
             self->init(true);
             if (autoWrite)
             {
                 self->write();
             }
        }, nb::arg("time"), nb::arg("autoWrite") = false)
        .def_static("fromPolyMesh",
            [](Foam::polyMesh& polyMesh, bool autoWrite) {
                return Foam::createMeshFromPolyMesh(polyMesh, autoWrite);
            },
            nb::arg("polyMesh"), nb::arg("autoWrite") = false,
            nb::rv_policy::take_ownership,
            "Create fvMesh from polyMesh by writing to disk and reading back")
        .def("nCells", [](const Foam::fvMesh& self)
        {
            return self.nCells();
        })
        .def("nFaces", [](const Foam::fvMesh& self)
        {
            return self.nFaces();
        })
        .def("nPoints", [](const Foam::fvMesh& self)
        {
            return self.nPoints();
        })
        .def("nInternalFaces", [](const Foam::fvMesh& self)
        {
            return self.nInternalFaces();
        })
        .def("time", &Foam::fvMesh::time, nb::rv_policy::reference)
        .def("C", &Foam::fvMesh::C, nb::rv_policy::reference)
        .def("V", [](const Foam::fvMesh &self) -> const Foam::Field<Foam::scalar>&
             { return static_cast<const Foam::Field<Foam::scalar>&>(self.V().field()); },
             nb::rv_policy::reference)
        .def("Cf", &Foam::fvMesh::Cf, nb::rv_policy::reference)
        .def("Sf", &Foam::fvMesh::Sf, nb::rv_policy::reference)
        .def("magSf", &Foam::fvMesh::magSf, nb::rv_policy::reference)
        .def("setFluxRequired", &Foam::fvMesh::setFluxRequired)
        .def("solverPerformanceDict", [](const Foam::fvMesh &self)
             {
                #if OPENFOAM >= 2312
                    return &self.data().solverPerformanceDict();
                #else
                    return &self.solverPerformanceDict();
                #endif

            },
             nb::rv_policy::reference)
        .def("boundary", [](const Foam::fvMesh &self) -> const Foam::fvBoundaryMesh& {
            return self.boundary();
        }, nb::rv_policy::reference_internal)
        .def("write", [](Foam::fvMesh& self) { return self.write(); },
             "Write mesh to disk")
        // dynamic mesh support
        .def("changing", [](Foam::fvMesh &self)
             { return self.changing(); })
        ;


        nb::class_<Foam::dynamicFvMesh, Foam::fvMesh>(m, "dynamicFvMesh")
        .def_static("New", [](
            const Foam::argList& args,
            const Foam::Time& runTime)
        {
            return Foam::dynamicFvMesh::New(args, runTime).ptr();
        }, nb::rv_policy::take_ownership)
        .def("updateMesh", &Foam::dynamicFvMesh::update)
        .def("controlledUpdateMesh", &Foam::dynamicFvMesh::controlledUpdate)
        .def("dynamic", &Foam::dynamicFvMesh::dynamic)

        ;
}
