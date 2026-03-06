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

#include "bind_wallDist.hpp"

namespace Foam
{

void bindWallDist(nanobind::module_& m)
{
    namespace nb = nanobind;

    nb::class_<wallDist>(m, "wallDist")
        .def_static("New", [](fvMesh& mesh) -> std::shared_ptr<const wallDist>
        {
            // wallDist has multiple inheritance (MeshObject<fvMesh, UpdateableMeshObject, wallDist>).
            // Use shared_ptr with a no-op deleter to safely handle base pointer offsets;
            // the mesh owns the wallDist singleton — Python must not delete it.
            return std::shared_ptr<const wallDist>(
                &wallDist::New(mesh),
                [](const wallDist*) {}  // no-op deleter: mesh owns the object
            );
        }, nb::arg("mesh"))
        .def("y", &wallDist::y, nb::rv_policy::reference)
        .def("n", &wallDist::n, nb::rv_policy::reference)
        ;

    nb::class_<nearWallDist>(m, "nearWallDist")
        .def(nb::init<const fvMesh&>(), nb::arg("mesh"))
        .def("correct", &nearWallDist::correct)
        ;

    nb::class_<nearWallDistNoSearch>(m, "nearWallDistNoSearch")
        .def(nb::init<const fvMesh&>(), nb::arg("mesh"))
        .def("correct", &nearWallDistNoSearch::correct)
        ;
}

} // namespace Foam
