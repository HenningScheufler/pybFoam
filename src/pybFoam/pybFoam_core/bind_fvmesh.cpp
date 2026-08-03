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

#include "bind_fvmesh.hpp"
#include "bind_time.hpp"
#include "bind_polymesh.hpp"
#include <memory>
#include <nanobind/make_iterator.h>
#include "volFields.H"
#include "surfaceFields.H"
#include "dynamicFvMesh.H"
#include "fvMesh.H"
#include "polyMesh.H"
#include "fvBoundaryMesh.H"
#include "fvPatch.H"
#include "IOobject.H"
#include "ITstream.H"
#include "ddtScheme.H"
#include "CrankNicolsonDdtScheme.H"

namespace Foam
{

    fvMesh *createMesh(const Time &time, bool autoWrite)
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
        if (autoWrite)
        {
            mesh->write();
        }
        return mesh;
    }

    fvMesh *createMeshFromPolyMesh(polyMesh& polyMeshRef, bool autoWrite)
    {
        // Write the polyMesh to disk
        polyMeshRef.write();

        // Get the time and location info from the polyMesh
        const Time& time = polyMeshRef.time();
        const word& regionName = polyMeshRef.name();

        // Create fvMesh by reading from disk where polyMesh was written
        fvMesh *mesh(
            new fvMesh(
                IOobject(
                    regionName,
                    time.timeName(),
                    time,
                    IOobject::MUST_READ),
                false));
        mesh->init(true);

        if (autoWrite)
        {
            mesh->write();
        }

        return mesh;
    }

}

void bindFvMesh(nanobind::module_ &m)
{
    using namespace Foam;
    namespace nb = nanobind;

    m.def("createMesh",
        [](const Foam::Time& time, bool autoWrite) {
            return Foam::createMesh(time, autoWrite);
        },
        nb::arg("time"), nb::arg("autoWrite") = false,
        nb::rv_policy::take_ownership,
        "Create a mesh from a Time object");

    nb::class_<Foam::fvBoundaryMesh>(m, "fvBoundaryMesh")
        .def("size", [](const Foam::fvBoundaryMesh& self) { return self.size(); })
        .def("__len__", [](const Foam::fvBoundaryMesh& self) { return self.size(); })
        .def("__getitem__", [](const Foam::fvBoundaryMesh& self, Foam::label i) -> const Foam::fvPatch& {
            return self[i];
        }, nb::rv_policy::reference_internal)
        .def("__iter__", [](const Foam::fvBoundaryMesh& self) {
            return nb::make_iterator(nb::type<Foam::fvBoundaryMesh>(), "iterator",
                self.begin(), self.end());
        }, nb::keep_alive<0, 1>())
        .def("findPatchID", [](const Foam::fvBoundaryMesh& self, const Foam::word& patchName) {
            return self.findPatchID(patchName);
        });

    nb::class_<Foam::fvPatch>(m, "fvPatch")
        .def("name", [](const Foam::fvPatch& self) -> const Foam::word& {
            return self.name();
        }, nb::rv_policy::reference)
        .def("size", [](const Foam::fvPatch& self) { return self.size(); })
        .def("start", [](const Foam::fvPatch& self) { return self.start(); })
        .def("index", [](const Foam::fvPatch& self) { return self.index(); })
        .def("coupled", [](const Foam::fvPatch& self) { return self.coupled(); });

    nb::class_<Foam::fvMesh>(m, "fvMesh")
        .def("__init__", [](Foam::fvMesh* self, const Foam::Time& time, bool autoWrite) {
             new (self) Foam::fvMesh(
                IOobject(
                    "region0",
                    time.timeName(),
                    time,
                    IOobject::MUST_READ),
                false);
             self->init(true);
             if (autoWrite)
             {
                 self->write();
             }
        }, nb::arg("time"), nb::arg("autoWrite") = false)
        .def_static("fromPolyMesh",
            [](Foam::polyMesh& polyMesh, bool autoWrite) {
                return Foam::createMeshFromPolyMesh(polyMesh, autoWrite);
            },
            nb::arg("polyMesh"), nb::arg("autoWrite") = false,
            nb::rv_policy::take_ownership,
            "Create fvMesh from polyMesh by writing to disk and reading back")
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
        .def("time", &Foam::fvMesh::time, nb::rv_policy::reference)
        .def("C", &Foam::fvMesh::C, nb::rv_policy::reference)
        .def("V", [](const Foam::fvMesh &self) -> const Foam::Field<Foam::scalar>&
             { return static_cast<const Foam::Field<Foam::scalar>&>(self.V().field()); },
             nb::rv_policy::reference)
        .def("Cf", &Foam::fvMesh::Cf, nb::rv_policy::reference)
        .def("Sf", &Foam::fvMesh::Sf, nb::rv_policy::reference)
        .def("magSf", &Foam::fvMesh::magSf, nb::rv_policy::reference)
        .def("setFluxRequired", &Foam::fvMesh::setFluxRequired)
        // The fvSchemes ddt entry a solver has to branch on rather than just
        // apply: interFoam's alphaEqn.H builds the scheme named by `key`
        // (falling back to fvSchemes' `default`) and reads the Crank-Nicolson
        // off-centring coefficient off it. Returns (scheme name, ocCoeff); the
        // coefficient is a Function1 of time, so it is evaluated at the current
        // time and is 0 for every scheme that has none.
        .def("ddtSchemeInfo", [](const Foam::fvMesh &self, const Foam::word &key)
             {
                Foam::ITstream is(self.ddtScheme(key));
                is.rewind();
                Foam::tmp<Foam::fv::ddtScheme<Foam::scalar>> tddt
                (
                    Foam::fv::ddtScheme<Foam::scalar>::New(self, is)
                );
                const Foam::fv::ddtScheme<Foam::scalar>& ddt = tddt();
                Foam::scalar ocCoeff = 0;
                if (Foam::isType<Foam::fv::CrankNicolsonDdtScheme<Foam::scalar>>(ddt))
                {
                    ocCoeff =
                        Foam::refCast
                        <
                            const Foam::fv::CrankNicolsonDdtScheme<Foam::scalar>
                        >(ddt).ocCoeff();
                }
                return nb::make_tuple(std::string(ddt.type()), ocCoeff);
             },
             nb::arg("key"),
             "The (scheme name, off-centring coefficient) of the fvSchemes ddt "
             "entry `key`, e.g. ddtSchemeInfo('ddt(alpha)') -> "
             "('CrankNicolson', 0.5). The coefficient is 0 for any scheme "
             "without one.")
        .def("solverPerformanceDict", [](const Foam::fvMesh &self)
             {
                #if OPENFOAM >= 2312
                    return &self.data().solverPerformanceDict();
                #else
                    return &self.solverPerformanceDict();
                #endif

            },
             nb::rv_policy::reference)
        .def("isFinalIteration", [](const Foam::fvMesh &self)
             {
                #if OPENFOAM >= 2312
                    return self.data().isFinalIteration();
                #else
                    return self.data().getOrDefault<bool>("finalIteration", false);
                #endif
             },
             "Whether solvers select the <field>Final solver settings")
        .def("setFinalIteration", [](Foam::fvMesh &self, bool on)
             {
                #if OPENFOAM >= 2312
                    self.data().setFinalIteration(on);
                #else
                    const_cast<Foam::data&>(self.data()).add("finalIteration", on, true);
                #endif
             },
             nb::arg("on"),
             "Mark the current iteration final so solvers pick the "
             "<field>Final solver settings (what pimpleControl::loop does)")
        .def("boundary", [](const Foam::fvMesh &self) -> const Foam::fvBoundaryMesh& {
            return self.boundary();
        }, nb::rv_policy::reference_internal)
        .def("write", [](Foam::fvMesh& self) { return self.write(); },
             "Write mesh to disk")
        // dynamic mesh support. All four are virtual on polyMesh/fvMesh, so a
        // plain fvMesh answers false/false and a dynamicFvMesh answers for its
        // motion solver — the caller never has to know which it holds.
        .def("changing", [](Foam::fvMesh &self)
             { return self.changing(); })
        .def("moving", [](const Foam::fvMesh &self)
             { return self.moving(); })
        .def("topoChanging", [](const Foam::fvMesh &self)
             { return self.topoChanging(); })
        .def("dynamic", [](const Foam::fvMesh &self)
             { return self.dynamic(); })
        // The mesh motion (swept-volume) flux, i.e. native's mesh.phi(). Only
        // a moving mesh has one; fvMesh::phi() calls FatalError otherwise, so
        // guard the call site with moving() exactly as the solvers do.
        .def("phi", [](const Foam::fvMesh &self) -> const Foam::surfaceScalarField&
             { return self.phi(); }, nb::rv_policy::reference_internal)
        ;


        nb::class_<Foam::dynamicFvMesh, Foam::fvMesh>(m, "dynamicFvMesh")
        .def_static("New", [](
            const Foam::argList& args,
            const Foam::Time& runTime)
            -> std::shared_ptr<Foam::dynamicFvMesh>
        {
            // With multiple inheritance, a base pointer may be offset from the allocation start;
            // deleting it via a raw pointer is UB. shared_ptr keeps the correct deleter/address.
            return std::shared_ptr<Foam::dynamicFvMesh>(
                Foam::dynamicFvMesh::New(args, runTime).release()
            );
        })
        .def("updateMesh", &Foam::dynamicFvMesh::update)
        .def("controlledUpdateMesh", &Foam::dynamicFvMesh::controlledUpdate)
        .def("dynamic", &Foam::dynamicFvMesh::dynamic)

        ;
}
