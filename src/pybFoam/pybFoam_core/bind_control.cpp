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

#include "bind_control.hpp"
#include "tmp.H"


namespace Foam
{

void bindControl(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<pisoControl>(m, "pisoControl")
        .def(py::init<fvMesh&, const word&>(), py::arg("mesh"), py::arg("dictName") = word("PISO"))
        .def("correct", &pisoControl::correct)
        .def("correctNonOrthogonal", &pisoControl::correctNonOrthogonal)
        .def("momentumPredictor", &pisoControl::momentumPredictor)
        .def("finalNonOrthogonalIter", &pisoControl::finalNonOrthogonalIter)
        .def("nNonOrthCorr", &pisoControl::nNonOrthCorr)
        .def("finalInnerIter", &pisoControl::finalInnerIter)
        ;

    py::class_<pimpleControl>(m, "pimpleControl")
        .def(py::init<fvMesh&, const word&>(), py::arg("mesh"), py::arg("dictName") = word("PIMPLE"))
        .def("correct", &pimpleControl::correct)
        .def("correctNonOrthogonal", &pimpleControl::correctNonOrthogonal)
        .def("momentumPredictor", &pimpleControl::momentumPredictor)
        .def("finalNonOrthogonalIter", &pimpleControl::finalNonOrthogonalIter)
        .def("nNonOrthCorr", &pimpleControl::nNonOrthCorr)
        .def("finalInnerIter", &pimpleControl::finalInnerIter)
        .def("loop", &pimpleControl::loop)
        .def("turbCorr", &pimpleControl::turbCorr)
        .def("finalIter", &pimpleControl::finalIter)
        ;

    py::class_<simpleControl>(m, "simpleControl")
        .def(py::init<fvMesh&, const word&>(), py::arg("mesh"), py::arg("dictName") = word("SIMPLE"))
        // .def("correct", &simpleControl::correct)
        ;
}

} // namespace Foam


