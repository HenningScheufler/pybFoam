# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2025 NeoFOAM authors

"""
Type stubs for pybFoam.vof — VoF (Volume of Fluid) bindings module.

This module exposes:
  - immiscibleIncompressibleTwoPhaseMixture  (mixture model)
  - TwoPhaseTransportModel                   (turbulence for two-phase flows)
  - solveAlpha                               (MULES-based alpha equation)
  - computeAlphaCourantNumber                (interface Co number)
"""

from __future__ import annotations

from pybFoam import (
    dimensionedScalar,
    fvMesh,
    fvVectorMatrix,
    surfaceScalarField,
    volScalarField,
    volVectorField,
)


class immiscibleIncompressibleTwoPhaseMixture:
    """
    Immiscible incompressible two-phase mixture model.

    Provides phase fractions alpha1/alpha2, phase densities, surface tension
    and interface properties. Corresponds to OpenFOAM's
    immiscibleIncompressibleTwoPhaseMixture class.
    """

    def __init__(self, U: volVectorField, phi: surfaceScalarField) -> None:
        """Construct from velocity and flux fields."""
        ...

    def alpha1(self) -> volScalarField:
        """Return phase-1 volume fraction field (reference)."""
        ...

    def alpha2(self) -> volScalarField:
        """Return phase-2 volume fraction field (reference)."""
        ...

    def rho1(self) -> dimensionedScalar:
        """Return phase-1 density."""
        ...

    def rho2(self) -> dimensionedScalar:
        """Return phase-2 density."""
        ...

    def nu(self) -> ...:
        """Return mixture kinematic viscosity field."""
        ...

    def cAlpha(self) -> float:
        """Return interface compression coefficient."""
        ...

    def nHatf(self) -> surfaceScalarField:
        """Return face-normal unit vector at interface (reference)."""
        ...

    def surfaceTensionForce(self) -> surfaceScalarField:
        """
        Return surface-tension body force as a face flux [N/m^2 * m^2].
        Used in the pressure equation as: phig = surfaceTensionForce() * rAUf * magSf
        """
        ...

    def correct(self) -> None:
        """Update transport and interface properties after solving alpha."""
        ...

    def read(self) -> bool:
        """Re-read transport properties from transportProperties dict."""
        ...


class TwoPhaseTransportModel:
    """
    Incompressible two-phase transport/turbulence model.

    Wraps incompressibleInterPhaseTransportModel<immiscibleIncompressibleTwoPhaseMixture>.
    """

    def __init__(
        self,
        rho: volScalarField,
        U: volVectorField,
        phi: surfaceScalarField,
        rhoPhi: surfaceScalarField,
        mixture: immiscibleIncompressibleTwoPhaseMixture,
    ) -> None:
        """Construct two-phase turbulence model."""
        ...

    def divDevRhoReff(
        self, rho: volScalarField, U: volVectorField
    ) -> fvVectorMatrix:
        """
        Return the effective viscous-stress divergence term for the momentum equation.
        Equivalent to OpenFOAM's turbulence->divDevRhoReff(rho, U).
        """
        ...

    def correct(self) -> None:
        """Correct the turbulence model (update k, epsilon/omega, nut, etc.)."""
        ...


def solveAlpha(
    alpha1: volScalarField,
    alpha2: volScalarField,
    phi: surfaceScalarField,
    rhoPhi: surfaceScalarField,
    rho: volScalarField,
    mixture: immiscibleIncompressibleTwoPhaseMixture,
) -> None:
    """
    Solve the volume-fraction (alpha) equation using MULES.

    Reads solver settings from ``mesh.solverDict(alpha1.name())``:

    * ``nAlphaCorr``      -- number of MULES corrector passes (default 1)
    * ``nAlphaSubCycles`` -- number of alpha sub-cycles (default 1)
    * ``MULESCorr``       -- enable implicit-upwind then MULES correction (default False)

    Updates *in-place*: ``alpha1``, ``alpha2``, ``rhoPhi``, ``rho``.
    Also calls ``mixture.correct()`` after each alpha corrector pass.
    """
    ...


def computeAlphaCourantNumber(
    phi: surfaceScalarField,
    alpha1: volScalarField,
) -> tuple[float, float]:
    """
    Compute interface (alpha) Courant number.

    Uses an interface-weighted flux: only faces where
    ``0.01 < interpolate(alpha1) < 0.99`` contribute.

    Returns:
        (maxAlphaCoNum, meanAlphaCoNum) — both as dimensionless floats.

    Note:
        Use ``pybFoam.computeCFLNumber(phi)`` for the bulk flow Courant number.
    """
    ...


def run_alpha_eqn(
    alpha1: volScalarField,
    alpha2: volScalarField,
    phi: surfaceScalarField,
    alpha_phi10: surfaceScalarField,
    mixture: immiscibleIncompressibleTwoPhaseMixture,
    n_alpha_corr: int = 1,
    mules_corr: bool = False,
    alpha_scheme: str = "div(phi,alpha)",
    alphar_scheme: str = "div(phirb,alpha)",
) -> None:
    """
    Run one pass of the alpha (phase-fraction) equation using MULES.

    This is the low-level per-sub-cycle solver.  It updates ``alpha1``,
    ``alpha2``, and ``alpha_phi10`` in-place.
    **The caller is responsible** for updating ``rhoPhi`` and ``rho``
    afterwards.

    Args:
        alpha1:        Phase-1 volume fraction (updated in-place).
        alpha2:        Phase-2 volume fraction (updated in-place, = 1 - alpha1).
        phi:           Face volumetric flux [m³/s].
        alpha_phi10:   Face alpha flux accumulator (updated in-place).
        mixture:       Immiscible two-phase mixture (provides cAlpha, nHatf).
        n_alpha_corr:  Number of MULES corrector passes (default 1).
        mules_corr:    If True, use implicit upwind predictor + MULES correction.
        alpha_scheme:  Convection scheme name for alpha (default 'div(phi,alpha)').
        alphar_scheme: Compression scheme name (default 'div(phirb,alpha)').
    """
    ...


# ---------------------------------------------------------------------------
# Low-level MULES primitives — mirror alphaEqn.H piece by piece
# ---------------------------------------------------------------------------


def compute_interface_compression_velocity(
    mixture: immiscibleIncompressibleTwoPhaseMixture,
    phi: surfaceScalarField,
) -> surfaceScalarField:
    """
    Compute interface compression velocity: phic = cAlpha * |phi / magSf|.

    Non-coupled boundary faces (inlets, outlets, walls) are zeroed so that
    compression only acts in the fluid interior and coupled patches.

    Returns a new ``surfaceScalarField`` named ``"phic"``.

    This corresponds to the phic computation at the top of ``alphaEqn.H``.
    """
    ...


def alpha_phase_flux(
    phi: surfaceScalarField,
    alpha1: volScalarField,
    alpha2: volScalarField,
    phic: surfaceScalarField,
    mixture: immiscibleIncompressibleTwoPhaseMixture,
    alpha_scheme: str = "div(phi,alpha)",
    alphar_scheme: str = "div(phirb,alpha)",
) -> surfaceScalarField:
    """
    Compute the scheme-based alpha face flux with interface compression.

    Computes::

        phir       = phic * mixture.nHatf()
        alphaPhiUn = fvc::flux(phi, alpha1, alpha_scheme)
                   + fvc::flux(-fvc::flux(-phir, alpha2, alphar_scheme),
                                alpha1, alphar_scheme)

    Returns a new ``surfaceScalarField`` named ``"alphaPhiUn"``.

    This mirrors the ``alphaPhiUn`` computation inside the ``nAlphaCorr``
    corrector loop of ``alphaEqn.H``.
    """
    ...


def mules_explicit_solve(
    alpha1: volScalarField,
    phi: surfaceScalarField,
    alpha_phi: surfaceScalarField,
) -> None:
    """
    MULES explicit solve with zero source terms (Sp = Su = 0).

    Calls::

        MULES::explicitSolve(1, alpha1, phi, alpha_phi, 0, 0, 1, 0)

    Updates ``alpha1`` and ``alpha_phi`` **in-place**.
    The caller must update ``alpha2 = 1 - alpha1`` and call
    ``mixture.correct()`` afterwards.

    Corresponds to the ``else`` branch inside the corrector loop of
    ``alphaEqn.H`` (``MULESCorr == false``).
    """
    ...


def mules_correct(
    alpha1: volScalarField,
    alpha_phi_un: surfaceScalarField,
    alpha_phi_corr: surfaceScalarField,
) -> None:
    """
    MULES correction step with zero source terms (Sp = 0).

    Calls::

        MULES::correct(1, alpha1, alpha_phi_un, alpha_phi_corr, 0, 0, 1, 0)

    **alpha_phi_corr must be initialised** to ``(alpha_phi_un - alpha_phi10)``
    by the caller *before* this call.
    After this call it contains the limited correction flux ready to be added
    to ``alpha_phi10``.

    Updates ``alpha1`` and ``alpha_phi_corr`` **in-place**.
    The caller must then do::

        if a_corr == 0:
            alpha_phi10 += alpha_phi_corr
        else:
            alpha1 = 0.5*alpha1 + 0.5*alpha10
            alpha_phi10 += 0.5*alpha_phi_corr

    Corresponds to the ``if (MULESCorr)`` branch inside the corrector loop of
    ``alphaEqn.H``.
    """
    ...


def mules_implicit_predictor(
    alpha1: volScalarField,
    phi: surfaceScalarField,
) -> surfaceScalarField:
    """
    Implicit upwind predictor for the MULESCorr branch.

    Builds and solves::

        fvmDdt(alpha1) + fvmDiv(phi_upwind, alpha1) = 0

    using Euler time discretisation and upwind spatial discretisation.
    Modifies ``alpha1`` **in-place** and returns the resulting upwind face
    flux as a new ``surfaceScalarField`` (to be stored as ``alpha_phi10``).

    Corresponds to the ``if (MULESCorr)`` predictor block before the
    corrector loop in ``alphaEqn.H``.
    """
    ...
