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

    void createTime(Time* self, const std::string& rootPath, const std::string& caseName)
    {
        new (self) Time(Time::controlDictName, rootPath, caseName);
    }

    void createTimeArgs(Time* self, const argList& args)
    {
        new (self) Time(Time::controlDictName, args);
    }

    void makeArgList(argList* self, const std::vector<std::string> &args)
    {
        int argc = args.size();
        char **argv = new char *[argc];
        for (int i = 0; i < argc; i++)
        {
            argv[i] = new char[args[i].size() + 1];
            strcpy(argv[i], args[i].c_str());
        }
        new (self) argList(argc, argv, true, true, true);
    }

}

void bindTime(nanobind::module_ &m)
{
    using namespace Foam;
    namespace nb = nanobind;

    m.def("selectTimes", &Foam::selectTimes);

    nb::class_<Foam::argList>(m, "argList")
        .def("__init__", &Foam::makeArgList)
        ;

    nb::class_<Foam::Time>(m, "Time")
        .def("__init__", &Foam::createTime)
        .def("__init__", &Foam::createTimeArgs)
        .def("setTime", [](
                            Foam::Time &self,
                            const Foam::instant &inst,
                            const Foam::label newIndex)
             { self.setTime(inst, newIndex); })
        .def("setDeltaT", [](Foam::Time &self, const Foam::scalar newDeltaT)
             { self.setDeltaT(newDeltaT); }, nb::arg("newDeltaT"))
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
             { return self.timeName(); }, nb::rv_policy::reference);
}
