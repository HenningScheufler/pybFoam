/*---------------------------------------------------------------------------*\
            Copyright (c) 2025, NeoFOAM authors
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

Description
    Binding for Foam::reconstructionSchemes — geometric (PLIC) interface
    reconstruction, the read-only half of isoAdvection used by interIsoFoam.

    Given a phase fraction ``alpha1`` it reconstructs a plane interface per
    surface cell (``isoAlpha`` / ``plicRDF``) WITHOUT advancing the solution:
    ``reconstruct()`` only recomputes the interface normals/centres, it never
    mutates ``alpha1``. This makes it suitable for post-processing — e.g.
    volume-conservative interface area, curvature and orientation diagnostics,
    or the sharp PLIC polygons themselves — on any (live or written) time step.

    The area-scaled normal field ``normal()`` carries the per-cell interface
    area in its magnitude (``mag(normal())``) and the unit interface normal in
    its direction. ``surface()`` returns the reconstructed interface as a
    meshed surface (per-cell polygons) together with a face->cell map.

\*---------------------------------------------------------------------------*/

#include "bind_reconstruction.hpp"

#include <stdexcept>

#include <nanobind/stl/string.h>
#include <nanobind/stl/unique_ptr.h>

#include "reconstructionSchemes.H"
#include "volFields.H"
#include "surfaceFields.H"

namespace nb = nanobind;

namespace Foam
{

// ---------------------------------------------------------------------------
// Owning wrapper. reconstructionSchemes is abstract (constructed via New,
// which returns an autoPtr to the selected isoAlpha/plicRDF model), so we
// hold the autoPtr and forward. The advector stores references to alpha1,
// phi and U; nb::keep_alive on the constructor keeps those fields alive for
// as long as this wrapper lives.
// ---------------------------------------------------------------------------
struct InterfaceReconstruction
{
    autoPtr<reconstructionSchemes> scheme;

    InterfaceReconstruction
    (
        volScalarField& alpha1,
        const surfaceScalarField& phi,
        const volVectorField& U,
        const word& schemeName
    )
    {
        // reconstructionSchemes::New selects the model from "reconstructionScheme"
        // and reads the same dict as its coefficients (isoFaceTol, surfCellTol,
        // ... all have defaults), so a one-key dict is sufficient.
        dictionary dict;
        dict.add("reconstructionScheme", schemeName);
        scheme = reconstructionSchemes::New(alpha1, phi, U, dict);
    }
};


void bindReconstruction(nb::module_ & m)
{
    using interface = reconstructionSchemes::interface;

    // The reconstructed PLIC interface: per-cell polygons (points are
    // disconnected) plus, for every face, the mesh cell it was cut from.
    nb::class_<interface>(m, "reconstructedInterface")
        .def("size", [](const interface& s) { return s.size(); },
            "Number of interface faces (one polygon per interface cell).")
        .def("points",
            [](const interface& s) -> const pointField& { return s.points(); },
            nb::rv_policy::reference_internal,
            "Surface points [m] (disconnected: each face owns its own points).")
        .def("face_centres",
            [](const interface& s) -> const vectorField& { return s.faceCentres(); },
            nb::rv_policy::reference_internal,
            "Face centres Cf [m].")
        .def("face_areas",
            [](const interface& s) -> const vectorField& { return s.faceAreas(); },
            nb::rv_policy::reference_internal,
            "Face area vectors Sf [m^2] (normal scaled by face area).")
        .def("mag_face_areas",
            [](const interface& s) -> const scalarField& { return s.magFaceAreas(); },
            nb::rv_policy::reference_internal,
            "Face area magnitudes |Sf| [m^2].")
        .def("face_normals",
            [](const interface& s) -> const vectorField& { return s.faceNormals(); },
            nb::rv_policy::reference_internal,
            "Unit face normals.")
        .def("area", [](const interface& s) { return sum(s.magFaceAreas()); },
            "Total reconstructed interface area [m^2].")
        .def("mesh_cells",
            [](const interface& s) -> const labelList& { return s.meshCells(); },
            nb::rv_policy::reference_internal,
            "For each interface face, the originating mesh cell index.")
    ;

    nb::class_<InterfaceReconstruction>(m, "reconstructionSchemes")
        .def(
            nb::init<volScalarField&, const surfaceScalarField&,
                     const volVectorField&, const word&>(),
            nb::arg("alpha1"), nb::arg("phi"), nb::arg("U"),
            nb::arg("scheme") = word("isoAlpha"),
            nb::keep_alive<1, 2>(), nb::keep_alive<1, 3>(), nb::keep_alive<1, 4>())
        // Recompute the interface (normals/centres) from the current alpha1.
        // Read-only w.r.t. alpha1 — safe to call every write step.
        .def("reconstruct",
            [](InterfaceReconstruction& self, bool force)
            { self.scheme->reconstruct(force); },
            nb::arg("force") = true,
            "Reconstruct the interface from alpha1 (does not modify alpha1).")
        // Area-scaled interface normal per cell: mag = interface area,
        // direction = interface normal.
        .def("normal",
            [](InterfaceReconstruction& self) -> const volVectorField&
            { return self.scheme->normal(); },
            nb::rv_policy::reference_internal,
            "Interface area-normal field (mag = per-cell interface area).")
        .def("centre",
            [](InterfaceReconstruction& self) -> const volVectorField&
            { return self.scheme->centre(); },
            nb::rv_policy::reference_internal,
            "Interface centre field (per-cell interface centroid).")
        .def("interface_cell",
            [](InterfaceReconstruction& self) -> const boolList&
            { return self.scheme->interfaceCell(); },
            nb::rv_policy::reference_internal,
            "Per-cell mask: True where the cell holds a reconstructed interface.")
        .def("surface",
            [](InterfaceReconstruction& self)
            { return self.scheme->surface(); },
            "Return the reconstructed interface as meshed polygons.")
    ;

    // Convenience: build a reconstruction directly from the mesh registry,
    // looking up alpha1 (by name), phi and U, and reconstruct once. Mirrors
    // interIsoFoam's field names; raises if a required field is not registered.
    m.def("reconstruct",
        [](fvMesh& mesh, const std::string& alphaName, const std::string& scheme)
        {
            if (!mesh.foundObject<volScalarField>(alphaName))
            {
                throw std::runtime_error(
                    "reconstruct: volScalarField '" + alphaName
                    + "' not found in mesh registry");
            }
            if (!mesh.foundObject<surfaceScalarField>("phi"))
            {
                throw std::runtime_error(
                    "reconstruct: surfaceScalarField 'phi' not found in mesh "
                    "registry (required to construct the reconstruction scheme)");
            }
            if (!mesh.foundObject<volVectorField>("U"))
            {
                throw std::runtime_error(
                    "reconstruct: volVectorField 'U' not found in mesh registry "
                    "(required to construct the reconstruction scheme)");
            }

            auto recon = std::make_unique<InterfaceReconstruction>(
                mesh.lookupObjectRef<volScalarField>(alphaName),
                mesh.lookupObject<surfaceScalarField>("phi"),
                mesh.lookupObject<volVectorField>("U"),
                word(scheme));
            recon->scheme->reconstruct(true);
            return recon;
        },
        nb::arg("mesh"), nb::arg("alpha") = "alpha.water",
        nb::arg("scheme") = "isoAlpha",
        // The returned reconstruction references registry-owned fields; keep
        // the mesh alive for as long as it lives.
        nb::keep_alive<0, 1>(),
        "Reconstruct the interface from a registered alpha field.\n\n"
        "Looks up ``alpha``, ``phi`` and ``U`` in the mesh registry, builds the\n"
        "chosen reconstruction scheme (default ``isoAlpha``) and calls\n"
        "``reconstruct()`` once. Returns a ``reconstructionSchemes`` wrapper.");
}

} // End namespace Foam

// ************************************************************************* //
