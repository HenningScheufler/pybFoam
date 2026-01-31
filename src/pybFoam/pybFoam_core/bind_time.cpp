/*---------------------------------------------------------------------------*\
            Copyright (c) 2022, Henning Scheufler
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

#include "bind_time.hpp"

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

void bindTime(pybind11::module &m)
{
    namespace py = pybind11;

    m.def("selectTimes", &Foam::selectTimes);

    py::class_<Foam::argList>(m, "argList")
        .def(py::init(&Foam::makeArgList), py::return_value_policy::take_ownership)
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
        .def("run", &Foam::Time::run)
        .def("write", &Foam::Time::write)
        .def("increment", [](Foam::Time &self)
             { self++; }
        )
        .def("printExecutionTime", [](Foam::Time &self)
             { self.printExecutionTime(Foam::Info); } 
            )
        .def("timeName", [](Foam::Time &self)
             { return self.timeName(); }, py::return_value_policy::reference);
}
