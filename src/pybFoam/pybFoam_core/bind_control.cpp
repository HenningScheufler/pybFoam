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

void bindControl(nanobind::module_& m)
{
    namespace nb = nanobind;

    nb::class_<pisoControl>(m, "pisoControl")
        .def(nb::init<fvMesh&, const word&>(), nb::arg("mesh"), nb::arg("dictName") = word("PISO"))
        .def("correct", &pisoControl::correct)
        .def("correctNonOrthogonal", &pisoControl::correctNonOrthogonal)
        .def("momentumPredictor", &pisoControl::momentumPredictor)
        .def("finalNonOrthogonalIter", &pisoControl::finalNonOrthogonalIter)
        .def("nNonOrthCorr", &pisoControl::nNonOrthCorr)
        .def("finalInnerIter", &pisoControl::finalInnerIter)
        ;

    nb::class_<pimpleControl>(m, "pimpleControl")
        .def(nb::init<fvMesh&, const word&>(), nb::arg("mesh"), nb::arg("dictName") = word("PIMPLE"))
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

    nb::class_<simpleControl>(m, "simpleControl")
        .def(nb::init<fvMesh&, const word&>(), nb::arg("mesh"), nb::arg("dictName") = word("SIMPLE"))
        .def("correctNonOrthogonal", &simpleControl::correctNonOrthogonal)
        .def("momentumPredictor", &simpleControl::momentumPredictor)
        .def("finalNonOrthogonalIter", &simpleControl::finalNonOrthogonalIter)
        .def("nNonOrthCorr", &simpleControl::nNonOrthCorr)
        .def("loop", &simpleControl::loop)
        ;
}

} // namespace Foam
