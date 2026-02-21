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

namespace Foam
{

void bindSampledSurface(nb::module_& m)
{
    nb::class_<sampledSurface>(m, "sampledSurface")
        .def("name", &sampledSurface::name,
            "Get the name of the surface")
        // .def("mesh",
        //     [](const sampledSurface& self) -> const polyMesh& {
        //         return self.mesh();
        //     },
        //     py::return_value_policy::reference,
        //     "Get reference to the mesh")
        .def("enabled", &sampledSurface::enabled,
            "Check if surface is enabled")
        .def("invariant", &sampledSurface::invariant,
            "Check if surface is invariant with geometry changes")
        .def("isPointData",
            [](const sampledSurface& self) { return self.isPointData(); },
            "Check if using interpolation to surface points")
        .def("needsUpdate", &sampledSurface::needsUpdate,
            "Check if the surface needs an update")
        .def("expire", &sampledSurface::expire,
            "Mark the surface as needing an update")
        .def("update", &sampledSurface::update,
            "Update the surface as required")
        .def("points", &sampledSurface::points,
            nb::rv_policy::reference,
            "Get points of surface")
        // .def("faces", &sampledSurface::faces,
        //     py::return_value_policy::reference,
        //     "Get faces of surface")
        .def("Sf", &sampledSurface::Sf,
            nb::rv_policy::reference,
            "Get face area vectors")
        .def("magSf", &sampledSurface::magSf,
            nb::rv_policy::reference,
            "Get face area magnitudes")
        .def("Cf", &sampledSurface::Cf,
            nb::rv_policy::reference,
            "Get face centres")
        .def("area", &sampledSurface::area,
            "Get total surface area")
        .def("hasFaceIds", &sampledSurface::hasFaceIds,
            "Check if element ids/order of original surface are available")
        // .def_static("New",
        //     [](const word& name, const fvMesh& mesh, const dictionary& dict) {
        //         return sampledSurface::New(name, mesh, dict).release();
        //     },
        //     py::arg("name"),
        //     py::arg("mesh"),
        //     py::arg("dict"),
        //     py::return_value_policy::take_ownership,
        //     "Construct a new sampledSurface from dictionary")
        .def_static("New",
            [](const word& name, const fvMesh& mesh, const dictionary& dict)
                -> std::shared_ptr<sampledSurface>
            {
                // With multiple inheritance, a base pointer may be offset from the allocation start;
                // deleting it via a raw pointer is UB. shared_ptr keeps the correct deleter/address.
                return std::shared_ptr<sampledSurface>(
                    sampledSurface::New(name, static_cast<const polyMesh&>(mesh), dict).release()
                );
            },
            nb::arg("name"),
            nb::arg("mesh"),
            nb::arg("dict"),
            "Construct a new sampledSurface from dictionary (fvMesh overload)");
}

void bindMeshSearch(nb::module_& m)
{
    nb::class_<meshSearch>(m, "meshSearch")
        .def("__init__", [](meshSearch* self, const fvMesh& mesh) {
                new (self) meshSearch(static_cast<const polyMesh&>(mesh));
            },
            nb::arg("mesh"),
            // nb::rv_policy::take_ownership,
            "Construct from fvMesh")
        // .def("mesh", &meshSearch::mesh,
        //     py::return_value_policy::reference,
        //     "Return reference to mesh")
        .def("findNearestCell",
            [](const meshSearch& self, const point& location, label seedCelli, bool useTreeSearch) {
                return self.findNearestCell(location, seedCelli, useTreeSearch);
            },
            nb::arg("location"),
            nb::arg("seedCelli") = -1,
            nb::arg("useTreeSearch") = true,
            "Find nearest cell to location")
        .def("findCell",
            [](const meshSearch& self, const point& location, label seedCelli, bool useTreeSearch) {
                return self.findCell(location, seedCelli, useTreeSearch);
            },
            nb::arg("location"),
            nb::arg("seedCelli") = -1,
            nb::arg("useTreeSearch") = true,
            "Find cell containing location");
}

void bindSampledSet(nb::module_& m)
{
    // Bind sampledSet directly without exposing coordSet separately
    nb::class_<sampledSet>(m, "sampledSet")
        // coordSet methods (inherited)
        .def("name",
            [](const sampledSet& self) -> const word& { return self.name(); },
            nb::rv_policy::reference_internal,
            "Get the name of the set")
        .def("axis",
            [](const sampledSet& self) -> const word& { return self.axis(); },
            nb::rv_policy::reference_internal,
            "Get the axis name (x, y, z, xyz, distance)")
        .def("points",
            [](const sampledSet& self) -> const pointField& { return self.points(); },
            nb::rv_policy::reference_internal,
            "Get the sampling points")
        .def("distance",
            [](const sampledSet& self) { return scalarField(self.distance()); },
            "Get cumulative distance along the set")
        .def("nPoints",
            [](const sampledSet& self) { return self.points().size(); },
            "Get number of points in the set")
        // sampledSet specific methods
        // .def("mesh",
        //     [](const sampledSet& self) -> const polyMesh& {
        //         return self.mesh();
        //     },
        //     py::return_value_policy::reference,
        //     "Get reference to the mesh")
        .def("searchEngine", &sampledSet::searchEngine,
            nb::rv_policy::reference,
            "Get reference to the mesh search engine")
        .def("segments", &sampledSet::segments,
            nb::rv_policy::reference_internal,
            "Get segment numbers for each point")
        .def("cells", &sampledSet::cells,
            nb::rv_policy::reference_internal,
            "Get cell IDs for each point")
        // .def("faces", &sampledSet::faces,
        //     py::return_value_policy::reference_internal,
        //     "Get face IDs for each point (-1 if not on face)")
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
            nb::arg("name"),
            nb::arg("mesh"),
            nb::arg("searchEngine"),
            nb::arg("dict"),
            nb::rv_policy::take_ownership,
            "Construct a new sampledSet from dictionary");
}

void bindInterpolation(nb::module_& m)
{
    // Scalar interpolation
    nb::class_<interpolation<scalar>>(
        m, "interpolationScalar")
        .def_static("New",
            [](const word& interpolationType, const volScalarField& vf) {
                return interpolation<scalar>::New(interpolationType, vf).ptr();
            },
            nb::arg("interpolationType"),
            nb::arg("field"),
            nb::rv_policy::take_ownership,
            "Create scalar interpolation scheme");

    // Vector interpolation
    nb::class_<interpolation<vector>>(
        m, "interpolationVector")
        .def_static("New",
            [](const word& interpolationType, const volVectorField& vf) {
                return interpolation<vector>::New(interpolationType, vf).ptr();
            },
            nb::arg("interpolationType"),
            nb::arg("field"),
            nb::rv_policy::take_ownership,
            "Create vector interpolation scheme");

    // Tensor interpolation
    nb::class_<interpolation<tensor>>(
        m, "interpolationTensor")
        .def_static("New",
            [](const word& interpolationType, const volTensorField& vf) {
                return interpolation<tensor>::New(interpolationType, vf).ptr();
            },
            nb::arg("interpolationType"),
            nb::arg("field"),
            nb::rv_policy::take_ownership,
            "Create tensor interpolation scheme");

    // SymmTensor interpolation
    nb::class_<interpolation<symmTensor>>(
        m, "interpolationSymmTensor")
        .def_static("New",
            [](const word& interpolationType, const volSymmTensorField& vf) {
                return interpolation<symmTensor>::New(interpolationType, vf).ptr();
            },
            nb::arg("interpolationType"),
            nb::arg("field"),
            nb::rv_policy::take_ownership,
            "Create symmTensor interpolation scheme");
}

void bindSamplingFunctions(nb::module_& m)
{
    // Scalar field sampling
    m.def("sampleOnFacesScalar",
        [](const sampledSurface& surface, const interpolation<scalar>& interpolator) {
            return scalarField(surface.sample(interpolator));
        },
        nb::arg("surface"),
        nb::arg("interpolator"),
        "Sample scalar field values onto surface faces");

    // Vector field sampling
    m.def("sampleOnFacesVector",
        [](const sampledSurface& surface, const interpolation<vector>& interpolator) {
            return vectorField(surface.sample(interpolator));
        },
        nb::arg("surface"),
        nb::arg("interpolator"),
        "Sample vector field values onto surface faces");

    // Tensor field sampling
    m.def("sampleOnFacesTensor",
        [](const sampledSurface& surface, const interpolation<tensor>& interpolator) {
            return tensorField(surface.sample(interpolator));
        },
        nb::arg("surface"),
        nb::arg("interpolator"),
        "Sample tensor field values onto surface faces");

    // SymmTensor field sampling
    m.def("sampleOnFacesSymmTensor",
        [](const sampledSurface& surface, const interpolation<symmTensor>& interpolator) {
            return symmTensorField(surface.sample(interpolator));
        },
        nb::arg("surface"),
        nb::arg("interpolator"),
        "Sample symmTensor field values onto surface faces");

    // Interpolate to points (if supported)
    m.def("sampleOnPointsScalar",
        [](const sampledSurface& surface, const interpolation<scalar>& interpolator) {
            return scalarField(surface.interpolate(interpolator));
        },
        nb::arg("surface"),
        nb::arg("interpolator"),
        "Interpolate scalar field values onto surface points");

    m.def("sampleOnPointsVector",
        [](const sampledSurface& surface, const interpolation<vector>& interpolator) {
            return vectorField(surface.interpolate(interpolator));
        },
        nb::arg("surface"),
        nb::arg("interpolator"),
        "Interpolate vector field values onto surface points");

    m.def("sampleOnPointsTensor",
        [](const sampledSurface& surface, const interpolation<tensor>& interpolator) {
            return tensorField(surface.interpolate(interpolator));
        },
        nb::arg("surface"),
        nb::arg("interpolator"),
        "Interpolate tensor field values onto surface points");

    m.def("sampleOnPointsSymmTensor",
        [](const sampledSurface& surface, const interpolation<symmTensor>& interpolator) {
            return symmTensorField(surface.interpolate(interpolator));
        },
        nb::arg("surface"),
        nb::arg("interpolator"),
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
        nb::arg("sampledSet"),
        nb::arg("interpolator"),
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
        nb::arg("sampledSet"),
        nb::arg("interpolator"),
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
        nb::arg("sampledSet"),
        nb::arg("interpolator"),
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
        nb::arg("sampledSet"),
        nb::arg("interpolator"),
        "Sample symmTensor field values onto sampledSet points");
}

} // End namespace Foam
