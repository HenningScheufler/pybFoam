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

#include "bind_mesh.hpp"
#include "volFields.H"
#include "surfaceFields.H"
#include "dynamicFvMesh.H"

namespace Foam
{

    Foam::instantList selectTimes(
        Time &runTime,
        const std::vector<std::string> &args)
    {
        int argc = args.size();
        char **argv = new char *[argc];
        for (int i = 0; i < argc; i++)
        {
            argv[i] = new char[args[i].size() + 1];
            strcpy(argv[i], args[i].c_str());
        }
        Foam::argList list_arg(argc, argv);
        return Foam::timeSelector::select0(runTime, list_arg);
    }

    Time *createTime(std::string rootPath, std::string caseName)
    {
        return new Time(rootPath, caseName);
    }

    Time* createTimeArgs(const argList& args)
    {
        // Create Time object with default controlDictName
        return new Time(Time::controlDictName, args);
    }

    fvMesh *createMesh(const Time &time)
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
        return mesh;
    }

    argList* makeArgList(
        const std::vector<std::string> &args)
    {
        // Keep original strings alive
        auto storage = std::make_shared<std::vector<std::string>>(args);
        auto argv_buf = std::make_shared<std::vector<char *>>();

        for (const auto &s : *storage)
            argv_buf->push_back(const_cast<char *>(s.c_str()));

        int argc = static_cast<int>(argv_buf->size());
        char **argv = argv_buf->data();

        // Pass to OpenFOAM argList
        return new argList(argc, argv, true, true, true);
    }

}

void bindMesh(pybind11::module &m)
{
    namespace py = pybind11;

    m.def("selectTimes", &Foam::selectTimes);

    py::class_<Foam::argList>(m, "argList")
        .def(py::init(&Foam::makeArgList), py::return_value_policy::take_ownership)
        // .def(py::init<const Foam::argList&, const Foam::HashTable<Foam::string>&, bool, bool, bool>())
        // .def("check", &Foam::argList::check)
        // .def("checkRootCase", &Foam::argList::checkRootCase)
        // .def("printCompat", &Foam::argList::printCompat)
        // .def("printNotes", &Foam::argList::printNotes)
        // .def("printUsage", &Foam::argList::printUsage)
        // .def("printMan", &Foam::argList::printMan)
        // .def("displayDoc", &Foam::argList::displayDoc)
        // .def("checkArgs", &Foam::argList::argsMandatory)
        // .def("setCheckArgs", &Foam::argList::argsMandatory, py::arg("check"))
        ;

    py::class_<Foam::Time>(m, "Time")
        .def(py::init([](const Foam::Time &self)
                      {
            Foam::Time& time = const_cast<Foam::Time&>(self);
            return &time; }),
             py::return_value_policy::reference_internal)
        .def(py::init(&Foam::createTime), py::return_value_policy::take_ownership)
        .def(py::init(&Foam::createTimeArgs), py::return_value_policy::take_ownership)
        .def("setTime", [](
                            Foam::Time &self,
                            const Foam::instant &inst,
                            const Foam::label newIndex)
             { self.setTime(inst, newIndex); })
        .def("setDeltaT", [](Foam::Time &self, const Foam::scalar newDeltaT)
             { self.setDeltaT(newDeltaT); }, py::arg("newDeltaT"))
        .def("value", &Foam::Time::timeOutputValue)
        .def("deltaTValue", [](Foam::Time &self) { return self.deltaTValue(); })
        .def("loop", &Foam::Time::loop)
        .def("write", &Foam::Time::write)
        .def("increment", [](Foam::Time &self)
             { self++; }
        )
        .def("printExecutionTime", [](Foam::Time &self)
             { self.printExecutionTime(Foam::Info); } 
            )
        .def("timeName", [](Foam::Time &self)
             { return self.timeName(); }, py::return_value_policy::reference);

    py::class_<Foam::fvMesh>(m, "fvMesh")
        .def(py::init([](const Foam::fvMesh &self)
                      {
            Foam::fvMesh& mesh = const_cast<Foam::fvMesh&>(self);
            return &mesh; }),
             py::return_value_policy::reference_internal)
        .def(py::init(&Foam::createMesh), py::return_value_policy::take_ownership)
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
        .def("Cf", &Foam::fvMesh::Cf, py::return_value_policy::reference)
        .def("Sf", &Foam::fvMesh::Sf, py::return_value_policy::reference)
        .def("magSf", &Foam::fvMesh::magSf, py::return_value_policy::reference)
        .def("setFluxRequired", &Foam::fvMesh::setFluxRequired)
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
