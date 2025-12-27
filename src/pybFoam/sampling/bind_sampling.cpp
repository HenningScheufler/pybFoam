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
                return sampledSurface::New(name, mesh, dict).release();
            },
            py::arg("name"),
            py::arg("mesh"),
            py::arg("dict"),
            py::return_value_policy::take_ownership,
            "Construct a new sampledSurface from dictionary")
        .def_static("New", 
            [](const word& name, const fvMesh& mesh, const dictionary& dict) {
                return sampledSurface::New(name, static_cast<const polyMesh&>(mesh), dict).release();
            },
            py::arg("name"),
            py::arg("mesh"),
            py::arg("dict"),
            py::return_value_policy::take_ownership,
            "Construct a new sampledSurface from dictionary (fvMesh overload)");
}

void bindMeshSearch(py::module& m)
{
    py::class_<meshSearch>(m, "meshSearch")
        .def(py::init([](const fvMesh& mesh) {
                return new meshSearch(static_cast<const polyMesh&>(mesh));
            }),
            py::arg("mesh"),
            py::return_value_policy::take_ownership,
            "Construct from fvMesh")
        .def("mesh", &meshSearch::mesh,
            py::return_value_policy::reference,
            "Return reference to mesh")
        .def("findNearestCell",
            py::overload_cast<const point&, const label, const bool>(
                &meshSearch::findNearestCell, py::const_),
            py::arg("location"),
            py::arg("seedCelli") = -1,
            py::arg("useTreeSearch") = true,
            "Find nearest cell to location")
        .def("findCell",
            py::overload_cast<const point&, const label, const bool>(
                &meshSearch::findCell, py::const_),
            py::arg("location"),
            py::arg("seedCelli") = -1,
            py::arg("useTreeSearch") = true,
            "Find cell containing location");
}

void bindSampledSet(py::module& m)
{
    // Bind sampledSet directly without exposing coordSet separately
    py::class_<sampledSet>(m, "sampledSet")
        // coordSet methods (inherited)
        .def("name", 
            [](const sampledSet& self) -> const word& { return self.name(); },
            py::return_value_policy::reference_internal,
            "Get the name of the set")
        .def("axis", 
            [](const sampledSet& self) -> const word& { return self.axis(); },
            py::return_value_policy::reference_internal,
            "Get the axis name (x, y, z, xyz, distance)")
        .def("points", 
            [](const sampledSet& self) -> const pointField& { return self.points(); },
            py::return_value_policy::reference_internal,
            "Get the sampling points")
        .def("distance", 
            [](const sampledSet& self) { return scalarField(self.distance()); },
            "Get cumulative distance along the set")
        .def("nPoints", 
            [](const sampledSet& self) { return self.points().size(); },
            "Get number of points in the set")
        // sampledSet specific methods
        .def("mesh", 
            [](const sampledSet& self) -> const polyMesh& {
                return self.mesh();
            },
            py::return_value_policy::reference,
            "Get reference to the mesh")
        .def("searchEngine", &sampledSet::searchEngine,
            py::return_value_policy::reference,
            "Get reference to the mesh search engine")
        .def("segments", &sampledSet::segments,
            py::return_value_policy::reference_internal,
            "Get segment numbers for each point")
        .def("cells", &sampledSet::cells,
            py::return_value_policy::reference_internal,
            "Get cell IDs for each point")
        .def("faces", &sampledSet::faces,
            py::return_value_policy::reference_internal,
            "Get face IDs for each point (-1 if not on face)")
        .def_static("New",
            [](const word& name, const fvMesh& mesh,
               meshSearch& searchEngine, const dictionary& dict) {
                return sampledSet::New(
                    name,
                    static_cast<const polyMesh&>(mesh),
                    searchEngine,
                    dict
                ).release();
            },
            py::arg("name"),
            py::arg("mesh"),
            py::arg("searchEngine"),
            py::arg("dict"),
            py::return_value_policy::take_ownership,
            "Construct a new sampledSet from dictionary");
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
        [](const sampledSurface& surface, const interpolation<scalar>& interpolator) -> scalarField {
            return surface.sample(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Sample scalar field values onto surface faces");

    // Vector field sampling
    m.def("sampleOnFacesVector",
        [](const sampledSurface& surface, const interpolation<vector>& interpolator) -> vectorField {
            return surface.sample(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Sample vector field values onto surface faces");

    // Tensor field sampling
    m.def("sampleOnFacesTensor",
        [](const sampledSurface& surface, const interpolation<tensor>& interpolator) -> tensorField {
            return surface.sample(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Sample tensor field values onto surface faces");

    // SymmTensor field sampling
    m.def("sampleOnFacesSymmTensor",
        [](const sampledSurface& surface, const interpolation<symmTensor>& interpolator) -> symmTensorField {
            return surface.sample(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Sample symmTensor field values onto surface faces");

    // Interpolate to points (if supported)
    m.def("sampleOnPointsScalar",
        [](const sampledSurface& surface, const interpolation<scalar>& interpolator) -> scalarField {
            return surface.interpolate(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Interpolate scalar field values onto surface points");

    m.def("sampleOnPointsVector",
        [](const sampledSurface& surface, const interpolation<vector>& interpolator) -> vectorField {
            return surface.interpolate(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Interpolate vector field values onto surface points");

    m.def("sampleOnPointsTensor",
        [](const sampledSurface& surface, const interpolation<tensor>& interpolator) -> tensorField {
            return surface.interpolate(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Interpolate tensor field values onto surface points");

    m.def("sampleOnPointsSymmTensor",
        [](const sampledSurface& surface, const interpolation<symmTensor>& interpolator) -> symmTensorField {
            return surface.interpolate(interpolator);
        },
        py::arg("surface"),
        py::arg("interpolator"),
        "Interpolate symmTensor field values onto surface points");

    // sampledSet sampling functions - manually sample at each point
    m.def("sampleSetScalar",
        [](const sampledSet& sSet, const interpolation<scalar>& interpolator) -> scalarField {
            scalarField values(sSet.nPoints());
            const pointField& points = sSet.points();
            const labelList& cells = sSet.cells();
            const labelList& faces = sSet.faces();
            
            forAll(sSet, samplei)
            {
                const point& p = points[samplei];
                const label celli = cells[samplei];
                const label facei = faces[samplei];
                
                if (celli == -1 && facei == -1)
                {
                    values[samplei] = pTraits<scalar>::max;
                }
                else
                {
                    values[samplei] = interpolator.interpolate(p, celli, facei);
                }
            }
            return values;
        },
        py::arg("sampledSet"),
        py::arg("interpolator"),
        "Sample scalar field values onto sampledSet points");

    m.def("sampleSetVector",
        [](const sampledSet& sSet, const interpolation<vector>& interpolator) -> vectorField {
            vectorField values(sSet.nPoints());
            const pointField& points = sSet.points();
            const labelList& cells = sSet.cells();
            const labelList& faces = sSet.faces();
            
            forAll(sSet, samplei)
            {
                const point& p = points[samplei];
                const label celli = cells[samplei];
                const label facei = faces[samplei];
                
                if (celli == -1 && facei == -1)
                {
                    values[samplei] = pTraits<vector>::max;
                }
                else
                {
                    values[samplei] = interpolator.interpolate(p, celli, facei);
                }
            }
            return values;
        },
        py::arg("sampledSet"),
        py::arg("interpolator"),
        "Sample vector field values onto sampledSet points");

    m.def("sampleSetTensor",
        [](const sampledSet& sSet, const interpolation<tensor>& interpolator) -> tensorField {
            tensorField values(sSet.nPoints());
            const pointField& points = sSet.points();
            const labelList& cells = sSet.cells();
            const labelList& faces = sSet.faces();
            
            forAll(sSet, samplei)
            {
                const point& p = points[samplei];
                const label celli = cells[samplei];
                const label facei = faces[samplei];
                
                if (celli == -1 && facei == -1)
                {
                    values[samplei] = pTraits<tensor>::max;
                }
                else
                {
                    values[samplei] = interpolator.interpolate(p, celli, facei);
                }
            }
            return values;
        },
        py::arg("sampledSet"),
        py::arg("interpolator"),
        "Sample tensor field values onto sampledSet points");

    m.def("sampleSetSymmTensor",
        [](const sampledSet& sSet, const interpolation<symmTensor>& interpolator) -> symmTensorField {
            symmTensorField values(sSet.nPoints());
            const pointField& points = sSet.points();
            const labelList& cells = sSet.cells();
            const labelList& faces = sSet.faces();
            
            forAll(sSet, samplei)
            {
                const point& p = points[samplei];
                const label celli = cells[samplei];
                const label facei = faces[samplei];
                
                if (celli == -1 && facei == -1)
                {
                    values[samplei] = pTraits<symmTensor>::max;
                }
                else
                {
                    values[samplei] = interpolator.interpolate(p, celli, facei);
                }
            }
            return values;
        },
        py::arg("sampledSet"),
        py::arg("interpolator"),
        "Sample symmTensor field values onto sampledSet points");
}

} // End namespace Foam
