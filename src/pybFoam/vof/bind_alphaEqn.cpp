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
    C++ wrapper for the VoF alpha equation (MULES-based phase-fraction
    advection) used in interFoam-family solvers.

    Implements the standard Euler-scheme alpha advection with optional
    interface compression and MULES correction (MULESCorr).

    Based on VoF/alphaEqn.H and VoF/alphaEqnSubCycle.H from OpenFOAM-v2406.

\*---------------------------------------------------------------------------*/

#include "bind_alphaEqn.hpp"

#include "immiscibleIncompressibleTwoPhaseMixture.H"
#include "CMULES.H"
#include "EulerDdtScheme.H"
#include "gaussConvectionScheme.H"
#include "upwind.H"
#include "subCycle.H"
#include "fvc.H"
#include "fvm.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "geometricOneField.H"
#include "zeroField.H"

namespace py = pybind11;

namespace Foam
{

// ---------------------------------------------------------------------------
// Run a single pass of the alpha equation for Euler scheme.
// Handles both MULESCorr=true and MULESCorr=false.
// Public: callable from Python via vof.run_alpha_eqn binding.
// ---------------------------------------------------------------------------
void runAlphaEqn
(
    volScalarField& alpha1,
    volScalarField& alpha2,
    const surfaceScalarField& phi,
    surfaceScalarField& alphaPhi10,
    immiscibleIncompressibleTwoPhaseMixture& mixture,
    int  nAlphaCorr,
    bool MULESCorr,
    const word&  alphaScheme,
    const word&  alpharScheme
)
{
    const fvMesh& mesh = alpha1.mesh();

    // Interface compression velocity: phic = cAlpha * |phi/magSf|
    surfaceScalarField phic
    (
        "phic",
        mixture.cAlpha() * mag(phi / mesh.magSf())
    );

    // Zero compression at non-coupled (inlet/outlet) boundaries
    surfaceScalarField::Boundary& phicBf = phic.boundaryFieldRef();
    forAll(phicBf, patchi)
    {
        if (!phicBf[patchi].coupled())
        {
            phicBf[patchi] == 0;
        }
    }

    if (MULESCorr)
    {
        // ---- Implicit upwind predictor ----
        fvScalarMatrix alpha1Eqn
        (
            fv::EulerDdtScheme<scalar>(mesh).fvmDdt(alpha1)
          + fv::gaussConvectionScheme<scalar>
            (
                mesh,
                phi,
                upwind<scalar>(mesh, phi)
            ).fvmDiv(phi, alpha1)
        );

        alpha1Eqn.solve();

        Info << "Phase-1 volume fraction (predictor) = "
             << alpha1.weightedAverage(mesh.Vsc()).value()
             << "  Min(" << alpha1.name() << ") = " << min(alpha1).value()
             << "  Max(" << alpha1.name() << ") = " << max(alpha1).value()
             << endl;

        // Store upwind flux as base
        alphaPhi10 = alpha1Eqn.flux();
    }

    // ---- Corrector loop ----
    volScalarField alpha10("alpha10", alpha1);

    for (int aCorr = 0; aCorr < nAlphaCorr; aCorr++)
    {
        // Relative compression flux
        surfaceScalarField phir("phir", phic * mixture.nHatf());

        // Scheme-based phase flux with interface compression
        surfaceScalarField alphaPhiUn
        (
            "alphaPhiUn",
            fvc::flux(phi, alpha1, alphaScheme)
          + fvc::flux
            (
               -fvc::flux(-phir, alpha2, alpharScheme),
                alpha1,
                alpharScheme
            )
        );

        if (MULESCorr)
        {
            // Correction flux relative to the upwind base
            tmp<surfaceScalarField> talphaPhi1Corr
            (
                alphaPhiUn - alphaPhi10
            );

            MULES::correct
            (
                geometricOneField(),
                alpha1,
                alphaPhiUn,
                talphaPhi1Corr.ref(),
                zeroField(),
                zeroField(),
                oneField(),
                zeroField()
            );

            if (aCorr == 0)
            {
                alphaPhi10 += talphaPhi1Corr();
            }
            else
            {
                alpha1 = 0.5*alpha1 + 0.5*alpha10;
                alphaPhi10 += 0.5*talphaPhi1Corr();
            }
        }
        else
        {
            alphaPhi10 = alphaPhiUn;

            MULES::explicitSolve
            (
                geometricOneField(),
                alpha1,
                phi,
                alphaPhi10,
                zeroField(),
                zeroField(),
                oneField(),
                zeroField()
            );
        }

        alpha2 = 1.0 - alpha1;
        mixture.correct();
    }

    Info << "Phase-1 volume fraction = "
         << alpha1.weightedAverage(mesh.Vsc()).value()
         << "  Min(" << alpha1.name() << ") = " << min(alpha1).value()
         << "  Max(" << alpha1.name() << ") = " << max(alpha1).value()
         << endl;
}


namespace
{
// ---------------------------------------------------------------------------
// Compute interface compression velocity phic = cAlpha * |phi/magSf|
// and zero out non-coupled (inlet/outlet) boundary face values.
// Returns a new named surfaceScalarField.
// ---------------------------------------------------------------------------
surfaceScalarField computeInterfaceCompressionVelocity
(
    immiscibleIncompressibleTwoPhaseMixture& mixture,
    const surfaceScalarField& phi
)
{
    const fvMesh& mesh = phi.mesh();

    surfaceScalarField phic
    (
        "phic",
        mixture.cAlpha() * mag(phi / mesh.magSf())
    );

    surfaceScalarField::Boundary& phicBf = phic.boundaryFieldRef();
    forAll(phicBf, patchi)
    {
        if (!phicBf[patchi].coupled())
        {
            phicBf[patchi] == 0;
        }
    }
    return phic;
}


// ---------------------------------------------------------------------------
// Compute scheme-based alpha phase flux with interface compression.
//
//   alphaPhiUn = fvc::flux(phi, alpha1, alphaScheme)
//              + fvc::flux(-fvc::flux(-phir, alpha2, alpharScheme),
//                           alpha1, alpharScheme)
//   where phir = phic * mixture.nHatf()
//
// This mirrors the inner body of the nAlphaCorr loop in alphaEqn.H.
// ---------------------------------------------------------------------------
surfaceScalarField alphaPhaseFlux
(
    const surfaceScalarField& phi,
    const volScalarField& alpha1,
    const volScalarField& alpha2,
    const surfaceScalarField& phic,
    immiscibleIncompressibleTwoPhaseMixture& mixture,
    const word& alphaScheme,
    const word& alpharScheme
)
{
    surfaceScalarField phir("phir", phic * mixture.nHatf());

    return surfaceScalarField
    (
        "alphaPhiUn",
        fvc::flux(phi, alpha1, alphaScheme)
      + fvc::flux
        (
           -fvc::flux(-phir, alpha2, alpharScheme),
            alpha1,
            alpharScheme
        )
    );
}


// ---------------------------------------------------------------------------
// MULES explicit solve (Sp = Su = 0 / zeroField).
// Modifies alpha1 and alphaPhi in-place.
//
//   MULES::explicitSolve(1, alpha1, phi, alphaPhi, 0, 0, 1, 0)
// ---------------------------------------------------------------------------
void mulesExplicitSolve
(
    volScalarField& alpha1,
    const surfaceScalarField& phi,
    surfaceScalarField& alphaPhi
)
{
    MULES::explicitSolve
    (
        geometricOneField(),
        alpha1,
        phi,
        alphaPhi,
        zeroField(),
        zeroField(),
        oneField(),
        zeroField()
    );
}


// ---------------------------------------------------------------------------
// MULES correction step (Sp = 0 / zeroField).
// Modifies alpha1 and alphaPhi1Corr in-place.
//
//   MULES::correct(1, alpha1, alphaPhiUn, alphaPhi1Corr, 0, 0, 1, 0)
//
// alphaPhi1Corr should be initialised to (alphaPhiUn - alphaPhi10) by
// the caller before this call.  After this call it contains the limited
// correction ready to be added to alphaPhi10.
// ---------------------------------------------------------------------------
void mulesCorrect
(
    volScalarField& alpha1,
    const surfaceScalarField& alphaPhiUn,
    surfaceScalarField& alphaPhi1Corr
)
{
    MULES::correct
    (
        geometricOneField(),
        alpha1,
        alphaPhiUn,
        alphaPhi1Corr,
        zeroField(),
        zeroField(),
        oneField(),
        zeroField()
    );
}


// ---------------------------------------------------------------------------
// Implicit upwind predictor for the MULESCorr branch.
// Builds and solves:
//   fvmDdt(alpha1) + fvmDiv(phi, alpha1) = 0   (upwind in space, Euler in time)
// Modifies alpha1 in-place and returns the resulting upwind flux.
// ---------------------------------------------------------------------------
surfaceScalarField mulesImplicitPredictor
(
    volScalarField& alpha1,
    const surfaceScalarField& phi
)
{
    const fvMesh& mesh = alpha1.mesh();

    fvScalarMatrix alpha1Eqn
    (
        fv::EulerDdtScheme<scalar>(mesh).fvmDdt(alpha1)
      + fv::gaussConvectionScheme<scalar>
        (
            mesh,
            phi,
            upwind<scalar>(mesh, phi)
        ).fvmDiv(phi, alpha1)
    );

    alpha1Eqn.solve();

    Info << "Phase-1 volume fraction (MULESCorr predictor) = "
         << alpha1.weightedAverage(mesh.Vsc()).value()
         << "  Min(" << alpha1.name() << ") = " << min(alpha1).value()
         << "  Max(" << alpha1.name() << ") = " << max(alpha1).value()
         << endl;

    return surfaceScalarField("alphaPhi10_pred", alpha1Eqn.flux());
}


// ---------------------------------------------------------------------------
// Top-level alpha sub-cycle solver (internal – not exported to Python directly).
// Reads solver settings from mesh.solverDict(alpha1.name()) and runs the
// alpha equation, including MULES subcycling.
// After solving, updates rhoPhi and rho.
// ---------------------------------------------------------------------------
void solveAlpha
(
    volScalarField&  alpha1,
    volScalarField&  alpha2,
    const surfaceScalarField& phi,
    surfaceScalarField& rhoPhi,
    volScalarField&  rho,
    immiscibleIncompressibleTwoPhaseMixture& mixture
)
{
    const fvMesh& mesh = alpha1.mesh();
    const dictionary& alphaDict = mesh.solverDict(alpha1.name());

    // Read algorithm parameters
    const int  nAlphaCorr      = alphaDict.getOrDefault<int>("nAlphaCorr", 1);
    const int  nAlphaSubCycles = alphaDict.getOrDefault<int>("nAlphaSubCycles", 1);
    const bool MULESCorr       = alphaDict.getOrDefault<bool>("MULESCorr", false);

    const word alphaScheme ("div(phi,alpha)");
    const word alpharScheme("div(phirb,alpha)");

    // Phase densities from mixture
    const dimensionedScalar& rho1 = mixture.rho1();
    const dimensionedScalar& rho2 = mixture.rho2();

    // Initialise phase-flux accumulator
    surfaceScalarField alphaPhi10
    (
        IOobject
        (
            "alphaPhi10",
            mesh.time().timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        phi * fvc::interpolate(alpha1)
    );

    if (nAlphaSubCycles > 1)
    {
        const dimensionedScalar totalDeltaT = mesh.time().deltaT();

        surfaceScalarField rhoPhiSum
        (
            IOobject
            (
                "rhoPhiSum",
                mesh.time().timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh,
            dimensionedScalar(rhoPhi.dimensions(), Zero)
        );

        // Interpolated face densities (uniform face values from dimensionedScalars)
        surfaceScalarField rho1f(IOobject("rho1f", mesh.time().timeName(), mesh, IOobject::NO_READ, IOobject::NO_WRITE), mesh, rho1);
        surfaceScalarField rho2f(IOobject("rho2f", mesh.time().timeName(), mesh, IOobject::NO_READ, IOobject::NO_WRITE), mesh, rho2);

        for
        (
            subCycle<volScalarField> alphaSubCycle(alpha1, nAlphaSubCycles);
            !(++alphaSubCycle).end();
        )
        {
            runAlphaEqn
            (
                alpha1, alpha2, phi, alphaPhi10, mixture,
                nAlphaCorr, MULESCorr, alphaScheme, alpharScheme
            );

            rhoPhiSum += (mesh.time().deltaT() / totalDeltaT)
                       * (alphaPhi10 * (rho1f - rho2f) + phi * rho2f);
        }

        rhoPhi = rhoPhiSum;
    }
    else
    {
        runAlphaEqn
        (
            alpha1, alpha2, phi, alphaPhi10, mixture,
            nAlphaCorr, MULESCorr, alphaScheme, alpharScheme
        );

        // Interpolated face densities for rhoPhi update (uniform face values)
        surfaceScalarField rho1f(IOobject("rho1f", mesh.time().timeName(), mesh, IOobject::NO_READ, IOobject::NO_WRITE), mesh, rho1);
        surfaceScalarField rho2f(IOobject("rho2f", mesh.time().timeName(), mesh, IOobject::NO_READ, IOobject::NO_WRITE), mesh, rho2);
        rhoPhi = alphaPhi10 * (rho1f - rho2f) + phi * rho2f;
    }

    // Update bulk density
    rho == alpha1 * rho1 + alpha2 * rho2;
}


// ---------------------------------------------------------------------------
// Compute alpha Courant number
// ---------------------------------------------------------------------------
std::tuple<scalar, scalar> computeAlphaCourantNumber
(
    const surfaceScalarField& phi,
    const volScalarField& alpha1
)
{
    const fvMesh& mesh = phi.mesh();
    const Time&   runTime = mesh.time();

    scalar alphaCoNum  = 0.0;
    scalar meanAlphaCo = 0.0;

    if (mesh.nInternalFaces())
    {
        // Use interface-weighted flux: interpolate alpha1 to faces first
        surfaceScalarField alphaf(fvc::interpolate(alpha1));

        scalarField sumPhiAlpha
        (
            fvc::surfaceSum
            (
                mag(phi)
              * pos0(alphaf - dimensionedScalar(dimless, scalar(0.01)))
              * pos0(dimensionedScalar(dimless, scalar(0.99)) - alphaf)
            )().primitiveField()
        );

        alphaCoNum = 0.5 * gMax(sumPhiAlpha / mesh.V().field())
                   * runTime.deltaTValue();

        meanAlphaCo = 0.5
                    * (gSum(sumPhiAlpha) / gSum(mesh.V().field()))
                    * runTime.deltaTValue();
    }

    return std::make_tuple(alphaCoNum, meanAlphaCo);
}

} // anonymous namespace


// ---------------------------------------------------------------------------
// Python bindings
// ---------------------------------------------------------------------------
void bindAlphaEqn(py::module& m)
{
    m.def(
        "solveAlpha",
        &solveAlpha,
        py::arg("alpha1"),
        py::arg("alpha2"),
        py::arg("phi"),
        py::arg("rhoPhi"),
        py::arg("rho"),
        py::arg("mixture"),
        "Solve alpha equation (MULES) with optional subcycling and MULESCorr.\n"
        "Reads nAlphaCorr, nAlphaSubCycles, MULESCorr from "
        "mesh.solverDict(alpha1.name()).\n"
        "Updates alpha1, alpha2, rhoPhi, and rho in-place."
    );

    m.def(
        "computeAlphaCourantNumber",
        &computeAlphaCourantNumber,
        py::arg("phi"),
        py::arg("alpha1"),
        "Compute Alpha Courant number (max and mean).\n"
        "Returns (alphaCoNum, meanAlphaCoNum)."
    );

    // Low-level binding: one pass of the alpha equation (no subcycling, no
    // rhoPhi/rho update). Allows Python-side orchestration of the MULES loop.
    m.def(
        "run_alpha_eqn",
        [](volScalarField& alpha1,
           volScalarField& alpha2,
           const surfaceScalarField& phi,
           surfaceScalarField& alphaPhi10,
           immiscibleIncompressibleTwoPhaseMixture& mixture,
           int nAlphaCorr,
           bool MULESCorr,
           const std::string& alphaScheme,
           const std::string& alpharScheme)
        {
            runAlphaEqn(alpha1, alpha2, phi, alphaPhi10, mixture,
                        nAlphaCorr, MULESCorr,
                        word(alphaScheme), word(alpharScheme));
        },
        py::arg("alpha1"),
        py::arg("alpha2"),
        py::arg("phi"),
        py::arg("alpha_phi10"),
        py::arg("mixture"),
        py::arg("n_alpha_corr")   = 1,
        py::arg("mules_corr")     = false,
        py::arg("alpha_scheme")   = "div(phi,alpha)",
        py::arg("alphar_scheme")  = "div(phirb,alpha)",
        "Run one pass of the alpha equation (MULES) without subcycling.\n"
        "Updates alpha1, alpha2, and alpha_phi10 in-place.\n"
        "rhoPhi and rho must be updated by the caller afterwards.\n"
        "Parameters:\n"
        "  n_alpha_corr  -- number of MULES corrector passes (default 1)\n"
        "  mules_corr    -- use implicit upwind predictor + MULES correction\n"
        "  alpha_scheme  -- convection scheme name for alpha (default 'div(phi,alpha)')\n"
        "  alphar_scheme -- compression scheme name (default 'div(phirb,alpha)')"
    );

    // -----------------------------------------------------------------------
    // Low-level primitives: each mirror one piece of alphaEqn.H so that
    // the full algorithm can be orchestrated in Python.
    // -----------------------------------------------------------------------

    m.def(
        "compute_interface_compression_velocity",
        [](immiscibleIncompressibleTwoPhaseMixture& mixture,
           const surfaceScalarField& phi)
        {
            return computeInterfaceCompressionVelocity(mixture, phi);
        },
        py::arg("mixture"), py::arg("phi"),
        "Compute interface compression velocity phic = cAlpha * |phi/magSf|.\n"
        "Non-coupled boundary faces are zeroed.  Returns a new surfaceScalarField."
    );

    m.def(
        "alpha_phase_flux",
        [](const surfaceScalarField& phi,
           const volScalarField& alpha1,
           const volScalarField& alpha2,
           const surfaceScalarField& phic,
           immiscibleIncompressibleTwoPhaseMixture& mixture,
           const std::string& alphaScheme,
           const std::string& alpharScheme)
        {
            return alphaPhaseFlux(phi, alpha1, alpha2, phic, mixture,
                                  word(alphaScheme), word(alpharScheme));
        },
        py::arg("phi"), py::arg("alpha1"), py::arg("alpha2"),
        py::arg("phic"), py::arg("mixture"),
        py::arg("alpha_scheme") = "div(phi,alpha)",
        py::arg("alphar_scheme") = "div(phirb,alpha)",
        "Compute scheme-based alpha flux with interface compression (alphaPhiUn).\n"
        "Returns a new surfaceScalarField.\n"
        "Mirrors the alphaPhiUn computation inside the nAlphaCorr loop of alphaEqn.H."
    );

    m.def(
        "mules_explicit_solve",
        [](volScalarField& alpha1,
           const surfaceScalarField& phi,
           surfaceScalarField& alphaPhi)
        {
            mulesExplicitSolve(alpha1, phi, alphaPhi);
        },
        py::arg("alpha1"), py::arg("phi"), py::arg("alpha_phi"),
        "MULES explicit solve: updates alpha1 and alpha_phi in-place (Sp=Su=0).\n"
        "Equivalent to MULES::explicitSolve(1, alpha1, phi, alpha_phi, 0, 0, 1, 0).\n"
        "alpha2 and mixture.correct() must be called by the Python caller."
    );

    m.def(
        "mules_correct",
        [](volScalarField& alpha1,
           const surfaceScalarField& alphaPhiUn,
           surfaceScalarField& alphaPhi1Corr)
        {
            mulesCorrect(alpha1, alphaPhiUn, alphaPhi1Corr);
        },
        py::arg("alpha1"), py::arg("alpha_phi_un"), py::arg("alpha_phi_corr"),
        "MULES correction step: updates alpha1 and alpha_phi_corr in-place (Sp=0).\n"
        "alpha_phi_corr must be initialised to (alphaPhiUn - alphaPhi10) before calling.\n"
        "After this call, alpha_phi_corr contains the limited correction to add to alphaPhi10.\n"
        "Equivalent to MULES::correct(1, alpha1, alpha_phi_un, alpha_phi_corr, 0, 0, 1, 0)."
    );

    m.def(
        "mules_implicit_predictor",
        [](volScalarField& alpha1,
           const surfaceScalarField& phi)
        {
            return mulesImplicitPredictor(alpha1, phi);
        },
        py::arg("alpha1"), py::arg("phi"),
        "Implicit upwind predictor for the MULESCorr branch.\n"
        "Solves fvmDdt(alpha1) + fvmDiv(phi_upwind, alpha1) = 0.\n"
        "Modifies alpha1 in-place and returns the resulting upwind face flux\n"
        "as a new surfaceScalarField (to be stored as alphaPhi10)."
    );
}

} // End namespace Foam

// ************************************************************************* //
