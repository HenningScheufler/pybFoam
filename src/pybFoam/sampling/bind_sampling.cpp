/*---------------------------------------------------------------------------*\
            Copyright (c) 2025, Henning Scheufler
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

#include "bind_sampling.hpp"
#include "volFields.H"
#include "surfaceFields.H"
#include "fvMesh.H"
#include <pybind11/functional.h>

namespace py = pybind11;

namespace Foam
{

void bindSampledSurface(py::module& m)
{
    py::class_<sampledSurface>(m, "sampledSurface")
        .def("name", &sampledSurface::name,
            "Get the name of the surface")
        .def("mesh", 
            [](const sampledSurface& self) -> const polyMesh& {
                return self.mesh();
            },
            py::return_value_policy::reference,
            "Get reference to the mesh")
        .def("enabled", &sampledSurface::enabled,
            "Check if surface is enabled")
        .def("invariant", &sampledSurface::invariant,
            "Check if surface is invariant with geometry changes")
        .def("isPointData", 
            py::overload_cast<>(&sampledSurface::isPointData, py::const_),
            "Check if using interpolation to surface points")
        .def("needsUpdate", &sampledSurface::needsUpdate,
            "Check if the surface needs an update")
        .def("expire", &sampledSurface::expire,
            "Mark the surface as needing an update")
        .def("update", &sampledSurface::update,
            "Update the surface as required")
        .def("points", &sampledSurface::points,
            py::return_value_policy::reference,
            "Get points of surface")
        .def("faces", &sampledSurface::faces,
            py::return_value_policy::reference,
            "Get faces of surface")
        .def("Sf", &sampledSurface::Sf,
            py::return_value_policy::reference,
            "Get face area vectors")
        .def("magSf", &sampledSurface::magSf,
            py::return_value_policy::reference,
            "Get face area magnitudes")
        .def("Cf", &sampledSurface::Cf,
            py::return_value_policy::reference,
            "Get face centres")
        .def("area", &sampledSurface::area,
            "Get total surface area")
        .def("hasFaceIds", &sampledSurface::hasFaceIds,
            "Check if element ids/order of original surface are available")
        .def_static("New", 
            [](const word& name, const polyMesh& mesh, const dictionary& dict) {
                return sampledSurface::New(name, mesh, dict).ptr();
            },
            py::arg("name"),
            py::arg("mesh"),
            py::arg("dict"),
            py::return_value_policy::take_ownership,
            "Construct a new sampledSurface from dictionary")
        .def_static("New", 
            [](const word& name, const fvMesh& mesh, const dictionary& dict) {
                return sampledSurface::New(name, static_cast<const polyMesh&>(mesh), dict).ptr();
            },
            py::arg("name"),
            py::arg("mesh"),
            py::arg("dict"),
            py::return_value_policy::take_ownership,
            "Construct a new sampledSurface from dictionary (fvMesh overload)");
}

void bindSampledPlane(py::module& m)
{
    py::class_<sampledPlane, sampledSurface>(m, "sampledPlane")
        .def(py::init<const word&, const polyMesh&, const dictionary&>(),
            py::arg("name"),
            py::arg("mesh"),
            py::arg("dict"),
            "Construct sampledPlane from name, mesh and dictionary");
}

void bindSampledPatch(py::module& m)
{
    py::class_<sampledPatch, sampledSurface>(m, "sampledPatch")
        .def(py::init<const word&, const polyMesh&, const dictionary&>(),
            py::arg("name"),
            py::arg("mesh"),
            py::arg("dict"),
            "Construct sampledPatch from name, mesh and dictionary");
}

void bindSampledCuttingPlane(py::module& m)
{
    py::class_<sampledCuttingPlane, sampledSurface>(m, "sampledCuttingPlane")
        .def(py::init<const word&, const polyMesh&, const dictionary&>(),
            py::arg("name"),
            py::arg("mesh"),
            py::arg("dict"),
            "Construct sampledCuttingPlane from name, mesh and dictionary");
}

void bindInterpolation(py::module& m)
{
    // Scalar interpolation
    py::class_<interpolation<scalar>>(
        m, "interpolationScalar")
        .def_static("New", 
            [](const word& interpolationType, const volScalarField& vf) {
                return interpolation<scalar>::New(interpolationType, vf).ptr();
            },
            py::arg("interpolationType"),
            py::arg("field"),
            py::return_value_policy::take_ownership,
            "Create scalar interpolation scheme");

    // Vector interpolation
    py::class_<interpolation<vector>>(
        m, "interpolationVector")
        .def_static("New", 
            [](const word& interpolationType, const volVectorField& vf) {
                return interpolation<vector>::New(interpolationType, vf).ptr();
            },
            py::arg("interpolationType"),
            py::arg("field"),
            py::return_value_policy::take_ownership,
            "Create vector interpolation scheme");

    // Tensor interpolation
    py::class_<interpolation<tensor>>(
        m, "interpolationTensor")
        .def_static("New", 
            [](const word& interpolationType, const volTensorField& vf) {
                return interpolation<tensor>::New(interpolationType, vf).ptr();
            },
            py::arg("interpolationType"),
            py::arg("field"),
            py::return_value_policy::take_ownership,
            "Create tensor interpolation scheme");

    // SymmTensor interpolation
    py::class_<interpolation<symmTensor>>(
        m, "interpolationSymmTensor")
        .def_static("New", 
            [](const word& interpolationType, const volSymmTensorField& vf) {
                return interpolation<symmTensor>::New(interpolationType, vf).ptr();
            },
            py::arg("interpolationType"),
            py::arg("field"),
            py::return_value_policy::take_ownership,
            "Create symmTensor interpolation scheme");
}

void bindSamplingFunctions(py::module& m)
{
    // Scalar field sampling
    m.def("sampleOnFacesScalar",
        [](const sampledSurface& surface, const interpolation<scalar>& interpolator) {
            return surface.sample(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Sample scalar field values onto surface faces");

    // Vector field sampling
    m.def("sampleOnFacesVector",
        [](const sampledSurface& surface, const interpolation<vector>& interpolator) {
            return surface.sample(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Sample vector field values onto surface faces");

    // Tensor field sampling
    m.def("sampleOnFacesTensor",
        [](const sampledSurface& surface, const interpolation<tensor>& interpolator) {
            return surface.sample(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Sample tensor field values onto surface faces");

    // SymmTensor field sampling
    m.def("sampleOnFacesSymmTensor",
        [](const sampledSurface& surface, const interpolation<symmTensor>& interpolator) {
            return surface.sample(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Sample symmTensor field values onto surface faces");

    // Interpolate to points (if supported)
    m.def("sampleOnPointsScalar",
        [](const sampledSurface& surface, const interpolation<scalar>& interpolator) {
            return surface.interpolate(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Interpolate scalar field values onto surface points");

    m.def("sampleOnPointsVector",
        [](const sampledSurface& surface, const interpolation<vector>& interpolator) {
            return surface.interpolate(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Interpolate vector field values onto surface points");

    m.def("sampleOnPointsTensor",
        [](const sampledSurface& surface, const interpolation<tensor>& interpolator) {
            return surface.interpolate(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Interpolate tensor field values onto surface points");

    m.def("sampleOnPointsSymmTensor",
        [](const sampledSurface& surface, const interpolation<symmTensor>& interpolator) {
            return surface.interpolate(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Interpolate symmTensor field values onto surface points");
}

} // End namespace Foam
