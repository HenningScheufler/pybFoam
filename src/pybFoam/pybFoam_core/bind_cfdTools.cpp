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

#include "bind_cfdTools.hpp"

#include "adjustPhi.H"
#include "findRefCell.H"
#include "constrainPressure.H"
#include "constrainHbyA.H"
#include "CorrectPhi.H"
#include "IOMRFZoneList.H"
#include "fvOptions.H"
#include "fvc.H"
#include <nanobind/stl/tuple.h>

namespace Foam
{

    std::tuple <scalar, scalar> computeCFLNumber
    (
        const surfaceScalarField& phi
    )
    {
        // Get the current time
        const fvMesh& mesh = phi.mesh();
        const Time& runTime = mesh.time();
        scalar CoNum = 0.0;
        scalar meanCoNum = 0.0;

        {
            scalarField sumPhi
            (
                fvc::surfaceSum(mag(phi))().primitiveField()
            );

            CoNum = 0.5*gMax(sumPhi/mesh.V().field())*runTime.deltaTValue();

            meanCoNum =
                0.5*(gSum(sumPhi)/gSum(mesh.V().field()))*runTime.deltaTValue();
        }

        return std::make_tuple(CoNum, meanCoNum);
    }

    std::tuple <scalar, scalar> computeContinuityErrors
    (
        const surfaceScalarField& phi
    )
    {
        // Get mesh and time
        const fvMesh& mesh = phi.mesh();
        const Time& runTime = mesh.time();

        // Compute divergence of flux
        volScalarField contErr(fvc::div(phi));

        // Calculate local continuity error (absolute sum)
        scalar sumLocalContErr = runTime.deltaTValue()*
            mag(contErr)().weightedAverage(mesh.V()).value();

        // Calculate global continuity error (signed)
        scalar globalContErr = runTime.deltaTValue()*
            contErr.weightedAverage(mesh.V()).value();

        return std::make_tuple(sumLocalContErr, globalContErr);
    }


    template <typename RAUType>
    void declare_constrainPressure(nanobind::module_ &m)
    {
        // Declare the function in the module
        namespace nb = nanobind;
        m.def("constrainPressure", [](volScalarField &p, const volVectorField &U, const surfaceScalarField &phiHbyA, const RAUType &rAU)
              { return constrainPressure(p, U, phiHbyA, rAU); }, nb::arg("p"), nb::arg("U"), nb::arg("phiHbyA"), nb::arg("rAU"));
        // The MRF-aware overload every rotating-frame solver calls
        // (simpleFoam/pimpleFoam/interFoam pEqn.H). It differs from the one above
        // only on fixedFluxPressure patches, where the prescribed snGrad must be
        // taken relative to the rotating frame.
        m.def("constrainPressure", [](volScalarField &p, const volVectorField &U, const surfaceScalarField &phiHbyA, const RAUType &rAU, const IOMRFZoneList &MRF)
              { return constrainPressure(p, U, phiHbyA, rAU, MRF); }, nb::arg("p"), nb::arg("U"), nb::arg("phiHbyA"), nb::arg("rAU"), nb::arg("MRF"));
    }


    // Foam::IOMRFZoneList — the rotating-zone list read from
    // constant/MRFProperties, bound as one flat class rather than as
    // IOdictionary + MRFZoneList bases: it inherits from both, so a bound base
    // would need nanobind to fix up the pointer offset on every cast. Every
    // method below is resolved in C++ inside the lambda, where the offsets are
    // the compiler's problem. `size` is the one name both bases carry, so it is
    // disambiguated explicitly.
    void declare_MRF(nanobind::module_ &m)
    {
        namespace nb = nanobind;

        nb::class_<IOMRFZoneList>(m, "IOMRFZoneList")
            // keep_alive<1,2>: the list keeps a bare reference to the mesh (and
            // registers itself on its registry), so the mesh must outlive it.
            .def(nb::init<const fvMesh &>(), nb::arg("mesh"), nb::keep_alive<1, 2>())
            .def("__len__", [](const IOMRFZoneList &self)
                 { return static_cast<const MRFZoneList &>(self).size(); })
            .def("active", [](const IOMRFZoneList &self, const bool warn)
                 { return self.active(warn); }, nb::arg("warn") = false)
            .def("DDt", [](const IOMRFZoneList &self, const volVectorField &U)
                 { return self.DDt(U); }, nb::arg("U"))
            .def("DDt", [](const IOMRFZoneList &self, const volScalarField &rho, const volVectorField &U)
                 { return self.DDt(rho, U); }, nb::arg("rho"), nb::arg("U"))
            .def("makeRelative", [](const IOMRFZoneList &self, surfaceScalarField &phi)
                 { self.makeRelative(phi); }, nb::arg("phi"))
            .def("makeAbsolute", [](const IOMRFZoneList &self, surfaceScalarField &phi)
                 { self.makeAbsolute(phi); }, nb::arg("phi"))
            .def("correctBoundaryVelocity", [](const IOMRFZoneList &self, volVectorField &U)
                 { self.correctBoundaryVelocity(U); }, nb::arg("U"))
            .def("zeroFilter", [](const IOMRFZoneList &self, const tmp<surfaceScalarField> &phi)
                 { return self.zeroFilter(phi); }, nb::arg("phi"))
            .def("update", [](IOMRFZoneList &self)
                 { self.update(); })
            ;
    }

    // Foam::fv::options — the finite-volume option list read from
    // constant/fvOptions or system/fvOptions (fvOptions.C picks whichever is
    // present, in that order). Bound flat, like IOMRFZoneList above: it inherits
    // both IOdictionary and fv::optionList, so every method is resolved in C++
    // inside the lambda where the base-pointer offsets are the compiler's
    // problem. `size` is carried by both bases, hence the explicit cast.
    //
    // Only the vector (momentum) arms are bound: the scalar equations OpenFOAM
    // would also source through fvOptions (k/epsilon/omega/he) are assembled
    // inside native library code, which reaches its own fv::options directly.
    void declare_fvOptions(nanobind::module_ &m)
    {
        namespace nb = nanobind;

        nb::class_<fv::options>(m, "fvOptions")
            // New() looks the list up on the mesh registry and constructs it
            // there on first call, so the returned reference is owned by the
            // mesh -- never by Python. keep_alive<0,1> holds the mesh for as
            // long as the handle lives; the native turbulence models call the
            // same New() and therefore share this very object.
            .def_static("New", [](const fvMesh &mesh) -> fv::options &
                 { return fv::options::New(mesh); },
                 nb::arg("mesh"), nb::rv_policy::reference, nb::keep_alive<0, 1>())
            .def("__len__", [](const fv::options &self)
                 { return static_cast<const fv::optionList &>(self).size(); })
            // fvOptions(U) / fvOptions(rho, U): the source matrix an equation
            // takes with `==`, i.e. subtracts.
            .def("__call__", [](fv::options &self, volVectorField &U)
                 { return self(U); }, nb::arg("U"))
            .def("__call__", [](fv::options &self, const volScalarField &rho, volVectorField &U)
                 { return self(rho, U); }, nb::arg("rho"), nb::arg("U"))
            .def("constrain", [](fv::options &self, fvMatrix<vector> &eqn)
                 { self.constrain(eqn); }, nb::arg("eqn"))
            .def("correct", [](fv::options &self, volVectorField &U)
                 { self.correct(U); }, nb::arg("U"))
            ;
    }

    void bindCfdTools(nanobind::module_ &m)
    {
        namespace nb = nanobind;

        m.def("adjustPhi", &adjustPhi);
        declare_MRF(m);
        declare_fvOptions(m);
        // The CorrectPhi projection itself is composed from primitives on the
        // Python side; only its one non-composable helper is bound.
        m.def("correctUphiBCs", [](volVectorField &U, surfaceScalarField &phi)
              { correctUphiBCs(U, phi); },
              nb::arg("U"), nb::arg("phi"));
        declare_constrainPressure<volScalarField>(m);
        declare_constrainPressure<surfaceScalarField>(m);
        m.def("constrainHbyA", &constrainHbyA);
        m.def("createPhi", [](const volVectorField &U)
        {
            const fvMesh& mesh = U.mesh();
            // Heap-allocate and hand ownership to the mesh registry (as
            // read_field does) so "phi" is discoverable by name — interFoam
            // registers phi, and the two-phase turbulence/mixture models look
            // it up by name. Returning a stack copy would leave it unregistered.
            surfaceScalarField* phi = new surfaceScalarField
            (
                IOobject
                (
                    "phi",
                    mesh.time().timeName(),
                    mesh,
                    IOobject::READ_IF_PRESENT,
                    IOobject::AUTO_WRITE
                ),
                fvc::flux(U)
            );
            mesh.objectRegistry::store(phi);
            return phi;
        }, nb::rv_policy::reference, nb::arg("U"));

        m.def("setRefCell", [](volScalarField &p, const Foam::dictionary &dict, const bool forceReference)
        {
            label pRefCell = 0;
            scalar pRefValue = 0.0;
            setRefCell(p, dict, pRefCell, pRefValue, forceReference);
            return std::make_tuple(pRefCell, pRefValue);
        }, nb::arg("p"), nb::arg("dict"), nb::arg("forceReference") = false);
        // Two-field variant setRefCell(p, p_rgh, dict) — used by VoF solvers
        // (interFoam) to reference the dynamic pressure p_rgh from the p keys.
        m.def("setRefCell", [](volScalarField &p, volScalarField &p_rgh, const Foam::dictionary &dict, const bool forceReference)
        {
            label pRefCell = 0;
            scalar pRefValue = 0.0;
            setRefCell(p, p_rgh, dict, pRefCell, pRefValue, forceReference);
            return std::make_tuple(pRefCell, pRefValue);
        }, nb::arg("p"), nb::arg("p_rgh"), nb::arg("dict"), nb::arg("forceReference") = false);
        m.def("computeCFLNumber", &computeCFLNumber);
        m.def("computeContinuityErrors", &computeContinuityErrors, nb::arg("phi"));
    }

} // namespace Foam
