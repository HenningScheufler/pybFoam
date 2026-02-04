/*---------------------------------------------------------------------------*\
            Copyright (c) 2021, Henning Scheufler
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

#include "bind_polymesh.hpp"
#include "polyMesh.H"
#include "polyBoundaryMesh.H"
#include "polyPatch.H"
#include "pointField.H"
#include "faceList.H"
#include "labelList.H"
#include "IOobject.H"
#include "fileName.H"
#include "cellShape.H"
#include "cellModel.H"
#include "Time.H"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace Foam
{

polyMesh* createPolyMesh(
    const IOobject& io,
    const pointField& points,
    const faceList& faces,
    const labelList& owner,
    const labelList& neighbour,
    bool syncPar)
{
    // Need to make copies because OpenFOAM requires rvalue references
    return new polyMesh(io, pointField(points), faceList(faces), labelList(owner), labelList(neighbour), syncPar);
}

polyMesh* createPolyMeshFromPython(
    const IOobject& io,
    const std::vector<std::vector<double>>& points,
    const std::vector<std::vector<Foam::label>>& faces,
    const std::vector<Foam::label>& owner,
    const std::vector<Foam::label>& neighbour,
    bool syncPar)
{
    // Convert Python types to OpenFOAM types
    pointField pts(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        pts[i] = Foam::vector(points[i][0], points[i][1], points[i][2]);
    }

    faceList fl(faces.size());
    for (size_t i = 0; i < faces.size(); ++i) {
        fl[i] = Foam::face(faces[i].size());
        for (size_t j = 0; j < faces[i].size(); ++j) {
            fl[i][j] = faces[i][j];
        }
    }

    labelList own(owner.size());
    for (size_t i = 0; i < owner.size(); ++i) {
        own[i] = owner[i];
    }

    labelList nei(neighbour.size());
    for (size_t i = 0; i < neighbour.size(); ++i) {
        nei[i] = neighbour[i];
    }

    return new polyMesh(io, std::move(pts), std::move(fl), std::move(own), std::move(nei), syncPar);
}

// Helper function to create polyMesh from cellShapes
polyMesh* createPolyMeshFromCellShapes(
    const IOobject& io,
    const std::vector<std::vector<double>>& points,
    const std::vector<std::tuple<std::string, std::vector<Foam::label>>>& cells,
    const std::vector<std::tuple<std::string, std::vector<std::vector<Foam::label>>>>& boundaryPatches,
    const std::string& defaultPatchName,
    bool syncPar)
{
    // Convert points
    pointField pts(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        pts[i] = Foam::vector(points[i][0], points[i][1], points[i][2]);
    }

    // Convert cells to cellShapes
    cellShapeList shapes(cells.size());
    for (size_t i = 0; i < cells.size(); ++i) {
        const auto& [cell_type, nodes] = cells[i];
        const cellModel& model = cellModel::ref(Foam::word(cell_type));

        labelList cellNodes(nodes.size());
        for (size_t j = 0; j < nodes.size(); ++j) {
            cellNodes[j] = nodes[j];
        }

        shapes[i].reset(model, cellNodes);
    }

    // Convert boundary patches
    const label nPatches = boundaryPatches.size();
    faceListList bFaces(nPatches);
    wordList patchNames(nPatches);
    wordList patchTypes(nPatches, polyPatch::typeName);

    for (label patchi = 0; patchi < nPatches; ++patchi) {
        const auto& [name, faces] = boundaryPatches[patchi];
        patchNames[patchi] = Foam::word(name);

        faceList pFaces(faces.size());
        for (size_t i = 0; i < faces.size(); ++i) {
            pFaces[i] = Foam::face(faces[i].size());
            for (size_t j = 0; j < faces[i].size(); ++j) {
                pFaces[i][j] = faces[i][j];
            }
        }
        bFaces[patchi] = pFaces;
    }

    wordList patchPhysicalTypes(nPatches, polyPatch::typeName);

    return new polyMesh(
        io,
        std::move(pts),
        shapes,
        bFaces,
        patchNames,
        patchTypes,
        Foam::word(defaultPatchName),
        polyPatch::typeName,
        patchPhysicalTypes,
        syncPar
    );
}

} // namespace Foam

void Foam::bindPolyMesh(pybind11::module &m)
{
    namespace py = pybind11;

    // fileName binding - simple string wrapper
    py::class_<Foam::fileName>(m, "fileName")
        .def(py::init<const std::string&>())
        .def("__str__", [](const Foam::fileName& self) { return std::string(self); });

    // IOobject bindings - bind enums first
    py::class_<Foam::IOobject> ioobject(m, "IOobject");

    py::enum_<Foam::IOobject::readOption>(ioobject, "readOption")
        .value("NO_READ", Foam::IOobject::NO_READ)
        .value("MUST_READ", Foam::IOobject::MUST_READ)
        .value("READ_IF_PRESENT", Foam::IOobject::READ_IF_PRESENT)
        .export_values();

    py::enum_<Foam::IOobject::writeOption>(ioobject, "writeOption")
        .value("NO_WRITE", Foam::IOobject::NO_WRITE)
        .value("AUTO_WRITE", Foam::IOobject::AUTO_WRITE)
        .export_values();

    ioobject.def(py::init<const Foam::word&, const Foam::fileName&, const Foam::Time&,
                      Foam::IOobject::readOption, Foam::IOobject::writeOption>(),
             py::arg("name"), py::arg("instance"), py::arg("registry"),
             py::arg("readOpt") = Foam::IOobject::NO_READ,
             py::arg("writeOpt") = Foam::IOobject::NO_WRITE);


    // polyMesh bindings
    py::class_<Foam::polyMesh>(m, "polyMesh")
        // Constructor from OpenFOAM types (low-level)
        .def(py::init(&Foam::createPolyMesh),
             py::arg("io"), py::arg("points"), py::arg("faces"),
             py::arg("owner"), py::arg("neighbour"), py::arg("syncPar") = true,
             py::return_value_policy::take_ownership,
             "Create polyMesh from OpenFOAM types")
        // Constructor from Python types (low-level)
        .def(py::init(&Foam::createPolyMeshFromPython),
             py::arg("io"), py::arg("points"), py::arg("faces"),
             py::arg("owner"), py::arg("neighbour"), py::arg("syncPar") = true,
             py::return_value_policy::take_ownership,
             "Create polyMesh from Python lists")
        // Constructor from cellShapes (high-level, handles orientation automatically)
        .def(py::init(&Foam::createPolyMeshFromCellShapes),
             py::arg("io"), py::arg("points"), py::arg("cells"),
             py::arg("boundaryPatches"), py::arg("defaultPatchName") = "defaultFaces",
             py::arg("syncPar") = true,
             py::return_value_policy::take_ownership,
             "Create polyMesh from cellShapes (handles face orientation automatically)")
        .def("write", [](Foam::polyMesh& self) { return self.write(); })
        .def("nCells", [](const Foam::polyMesh& self)
        {
            return self.nCells();
        })
        .def("nFaces", [](const Foam::polyMesh& self)
        {
            return self.nFaces();
        })
        .def("nPoints", [](const Foam::polyMesh& self)
        {
            return self.nPoints();
        })
        .def("nInternalFaces", [](const Foam::polyMesh& self)
        {
            return self.nInternalFaces();
        })
        .def("facesInstance", &Foam::polyMesh::facesInstance)
        .def_property_readonly("meshSubDir", [](const Foam::polyMesh& self) { return self.meshSubDir; })
        .def("boundaryMesh", &Foam::polyMesh::boundaryMesh, py::return_value_policy::reference)
        .def("points", [](const Foam::polyMesh& self) -> const Foam::pointField& {
            return self.points();
        }, py::return_value_policy::reference_internal)
        .def("faces", [](const Foam::polyMesh& self) -> const Foam::faceList& {
            return self.faces();
        }, py::return_value_policy::reference_internal)
        .def("owner", [](const Foam::polyMesh& self) -> const Foam::labelList& {
            return self.faceOwner();
        }, py::return_value_policy::reference_internal)
        .def("neighbour", [](const Foam::polyMesh& self) -> const Foam::labelList& {
            return self.faceNeighbour();
        }, py::return_value_policy::reference_internal)
        .def("removeBoundary", &Foam::polyMesh::removeBoundary, "Remove boundary patches from mesh")
        .def("addPatches", [](Foam::polyMesh& self, const std::vector<Foam::polyPatch*>& patches, bool validBoundary) {
            Foam::PtrList<Foam::polyPatch> patchList(patches.size());
            for (size_t i = 0; i < patches.size(); ++i) {
                patchList.set(i, patches[i]);
            }
            self.addPatches(patchList, validBoundary);
        }, py::arg("patches"), py::arg("validBoundary") = true);

    py::class_<Foam::polyBoundaryMesh>(m, "polyBoundaryMesh")
        .def("size", [](const Foam::polyBoundaryMesh& self) { return self.size(); })
        .def("__len__", [](const Foam::polyBoundaryMesh& self) { return self.size(); })
        .def("__getitem__", [](const Foam::polyBoundaryMesh& self, Foam::label i) -> const Foam::polyPatch& {
            return self[i];
        }, py::return_value_policy::reference_internal)
        .def("findPatchID", [](const Foam::polyBoundaryMesh& self, const Foam::word& patchName) {
            return self.findPatchID(patchName);
        });

    py::class_<Foam::polyPatch>(m, "polyPatch")
        .def(py::init([](const Foam::word& name, Foam::label size, Foam::label start,
                         Foam::label index, const Foam::polyBoundaryMesh& bm, const Foam::word& patchType) {
            return new Foam::polyPatch(name, size, start, index, bm, patchType);
        }), py::arg("name"), py::arg("size"), py::arg("start"), py::arg("index"),
            py::arg("boundaryMesh"), py::arg("patchType"),
            py::return_value_policy::take_ownership)
        .def("name", [](const Foam::polyPatch& self) -> const Foam::word& {
            return self.name();
        }, py::return_value_policy::reference)
        .def("size", [](const Foam::polyPatch& self) { return self.size(); })
        .def("start", [](const Foam::polyPatch& self) { return self.start(); })
        .def("type", [](const Foam::polyPatch& self) -> const Foam::word& {
            return self.type();
        }, py::return_value_policy::reference);
}
