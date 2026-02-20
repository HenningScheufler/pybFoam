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
#include "volFields.H"
#include "surfaceFields.H"
#include "dynamicFvMesh.H"
#include "fvMesh.H"
#include "polyMesh.H"
#include "fvBoundaryMesh.H"
#include "fvPatch.H"
#include "IOobject.H"
#include "fvSolution.H"

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

void bindFvMesh(pybind11::module &m)
{
    namespace py = pybind11;

    m.def("createMesh",
        [](const Foam::Time& time, bool autoWrite) {
            return Foam::createMesh(time, autoWrite);
        },
        py::arg("time"), py::arg("autoWrite") = false,
        py::return_value_policy::take_ownership,
        "Create a mesh from a Time object");

    py::class_<Foam::fvBoundaryMesh>(m, "fvBoundaryMesh")
        .def("size", [](const Foam::fvBoundaryMesh& self) { return self.size(); })
        .def("__len__", [](const Foam::fvBoundaryMesh& self) { return self.size(); })
        .def("__getitem__", [](const Foam::fvBoundaryMesh& self, Foam::label i) -> const Foam::fvPatch& {
            return self[i];
        }, py::return_value_policy::reference_internal)
        .def("findPatchID", [](const Foam::fvBoundaryMesh& self, const Foam::word& patchName) {
            return self.findPatchID(patchName);
        });

    py::class_<Foam::fvPatch>(m, "fvPatch")
        .def("name", [](const Foam::fvPatch& self) -> const Foam::word& {
            return self.name();
        }, py::return_value_policy::reference)
        .def("size", [](const Foam::fvPatch& self) { return self.size(); })
        .def("start", [](const Foam::fvPatch& self) { return self.start(); })
        .def("index", [](const Foam::fvPatch& self) { return self.index(); });

    py::class_<Foam::fvMesh>(m, "fvMesh")
        .def(py::init([](const Foam::fvMesh &self)
                      {
            Foam::fvMesh& mesh = const_cast<Foam::fvMesh&>(self);
            return &mesh; }),
             py::return_value_policy::reference_internal)
        .def(py::init([](const Foam::Time& time, bool autoWrite) {
                 return Foam::createMesh(time, autoWrite);
             }),
             py::arg("time"), py::arg("autoWrite") = false,
             py::return_value_policy::take_ownership)
        .def_static("fromPolyMesh",
            [](Foam::polyMesh& polyMesh, bool autoWrite) {
                return Foam::createMeshFromPolyMesh(polyMesh, autoWrite);
            },
            py::arg("polyMesh"), py::arg("autoWrite") = false,
            py::return_value_policy::take_ownership,
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
        .def("time", &Foam::fvMesh::time, py::return_value_policy::reference)
        .def("C", &Foam::fvMesh::C, py::return_value_policy::reference)
        .def("V", [](const Foam::fvMesh &self) -> const Foam::Field<Foam::scalar>&
             { return static_cast<const Foam::Field<Foam::scalar>&>(self.V().field()); },
             py::return_value_policy::reference)
        .def("Cf", &Foam::fvMesh::Cf, py::return_value_policy::reference)
        .def("Sf", &Foam::fvMesh::Sf, py::return_value_policy::reference)
        .def("magSf", &Foam::fvMesh::magSf, py::return_value_policy::reference)
        .def("setFluxRequired", &Foam::fvMesh::setFluxRequired)
        .def("solverPerformanceDict", [](const Foam::fvMesh &self)
             {
                #if OPENFOAM >= 2312
                    return &self.data().solverPerformanceDict();
                #else
                    return &self.solverPerformanceDict();
                #endif

            },
             py::return_value_policy::reference)
        .def("boundary", [](const Foam::fvMesh &self) -> const Foam::fvBoundaryMesh& {
            return self.boundary();
        }, py::return_value_policy::reference_internal)
        // Add polyMesh access methods
        .def("points", [](const Foam::fvMesh& self) -> const Foam::pointField& {
            return self.points();
        }, py::return_value_policy::reference_internal,
           "Get mesh points")
        .def("faces", [](const Foam::fvMesh& self) -> const Foam::faceList& {
            return self.faces();
        }, py::return_value_policy::reference_internal,
           "Get mesh faces")
        .def("owner", [](const Foam::fvMesh& self) -> const Foam::labelList& {
            return self.faceOwner();
        }, py::return_value_policy::reference_internal,
           "Get face owner cells")
        .def("neighbour", [](const Foam::fvMesh& self) -> const Foam::labelList& {
            return self.faceNeighbour();
        }, py::return_value_policy::reference_internal,
           "Get face neighbour cells")
        .def("write", [](Foam::fvMesh& self) { return self.write(); },
             "Write mesh to disk")
        // Solver settings lookup (reads from system/fvSolution.solvers.<name>)
        .def("solverDict",
            [](const Foam::fvMesh& self, const std::string& name) -> const Foam::dictionary& {
                return self.solution().solverDict(Foam::word(name));
            },
            py::return_value_policy::reference_internal,
            "Return solver dictionary for the named field from fvSolution.solvers")
        // dynamic mesh support
        .def("changing", [](Foam::fvMesh &self)
             { return self.changing(); })
        ;


        py::class_<Foam::dynamicFvMesh, Foam::fvMesh>(m, "dynamicFvMesh")
        .def_static("New", [](
            const Foam::argList& args,
            const Foam::Time& runTime)
        {
            return Foam::dynamicFvMesh::New(args, runTime).ptr();
        }, py::return_value_policy::take_ownership)
        .def("updateMesh", &Foam::dynamicFvMesh::update)
        .def("controlledUpdateMesh", &Foam::dynamicFvMesh::controlledUpdate)
        .def("dynamic", &Foam::dynamicFvMesh::dynamic)

        ;
}
