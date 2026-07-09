# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2025 NeoFOAM authors

"""
Type stubs for pybFoam.multiphase — multiphase (VoF) bindings module.

This module exposes:
  - immiscibleIncompressibleTwoPhaseMixture  (mixture model)
  - TwoPhaseTransportModel                   (turbulence for two-phase flows)
  - isoAdvection                             (geometric VoF advection)
  - reconstructionSchemes                    (geometric PLIC reconstruction)
"""

from __future__ import annotations

from pybFoam import (
    boolList,
    dimensionedScalar,
    fvMesh,
    fvVectorMatrix,
    labelList,
    scalarField,
    surfaceScalarField,
    tmp_volScalarField,
    vectorField,
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

    def nu(self) -> tmp_volScalarField:
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


class isoAdvection:
    """
    Geometric VoF advection (isoAdvector, as used by interIsoFoam).

    Reconstructs the interface (``reconstructionScheme``, e.g. isoAlpha) and
    advects ``alpha1`` in place. Controls are read from the case's
    ``system/fvSolution`` ``solvers.<alpha1>`` sub-dict at construction, so the
    advector is built once and ``advect`` is called each outer corrector.
    """

    def __init__(
        self,
        alpha1: volScalarField,
        phi: surfaceScalarField,
        U: volVectorField,
    ) -> None:
        """Construct from the phase fraction, face flux and velocity fields."""
        ...

    def advect(self) -> None:
        """Advect the interface, updating ``alpha1`` in place (zero Sp/Su)."""
        ...

    def get_rho_phi(
        self, rho1: dimensionedScalar, rho2: dimensionedScalar
    ) -> surfaceScalarField:
        """Return the density-weighted face flux ``(rho1-rho2)*alphaPhi + rho2*phi``."""
        ...

    def alpha_phi(self) -> surfaceScalarField:
        """Return the bounded phase face flux ``alphaPhi`` (reference)."""
        ...


class reconstructedInterface:
    """
    Reconstructed PLIC interface as meshed polygons.

    One polygon per interface cell (the points are *disconnected* — each face
    owns its own points). ``mesh_cells`` maps every interface face back to the
    mesh cell it was cut from. Returned by :meth:`reconstructionSchemes.surface`.
    """

    def size(self) -> int:
        """Number of interface faces (one polygon per interface cell)."""
        ...

    def points(self) -> vectorField:
        """Surface points [m] (disconnected: each face owns its own points)."""
        ...

    def face_centres(self) -> vectorField:
        """Face centres ``Cf`` [m]."""
        ...

    def face_areas(self) -> vectorField:
        """Face area vectors ``Sf`` [m^2] (normal scaled by face area)."""
        ...

    def mag_face_areas(self) -> scalarField:
        """Face area magnitudes ``|Sf|`` [m^2]."""
        ...

    def face_normals(self) -> vectorField:
        """Unit face normals."""
        ...

    def area(self) -> float:
        """Total reconstructed interface area [m^2]."""
        ...

    def mesh_cells(self) -> labelList:
        """For each interface face, the originating mesh cell index."""
        ...


class reconstructionSchemes:
    """
    Geometric (PLIC) interface reconstruction — the read-only half of
    ``isoAdvection`` (as used by interIsoFoam).

    Given a phase fraction ``alpha1`` it reconstructs a plane interface per
    surface cell (``isoAlpha`` or ``plicRDF``). :meth:`reconstruct` only
    recomputes the interface normals/centres — it never mutates ``alpha1`` —
    so it is safe for post-processing on any (live or written) time step.

    Note:
        Constructs mesh-registered ``interfaceNormal.<phase>`` /
        ``interfaceCentre.<phase>`` fields, so build **one** instance per mesh
        and reuse it (call :meth:`reconstruct` each step). ``plicRDF`` also
        reads the flux ``phi``; ``isoAlpha`` ignores it.
    """

    def __init__(
        self,
        alpha1: volScalarField,
        phi: surfaceScalarField,
        U: volVectorField,
        scheme: str = "isoAlpha",
    ) -> None:
        """Construct from the phase fraction, face flux and velocity fields."""
        ...

    def reconstruct(self, force: bool = True) -> None:
        """Reconstruct the interface from ``alpha1`` (does not modify ``alpha1``)."""
        ...

    def normal(self) -> volVectorField:
        """
        Interface area-normal field (reference).

        The magnitude is the per-cell interface area and the direction is the
        interface normal, so ``mag(normal())`` is the per-cell interface area
        and ``sum(mag(normal()))`` the total interface area.
        """
        ...

    def centre(self) -> volVectorField:
        """Interface centre field — per-cell interface centroid (reference)."""
        ...

    def interface_cell(self) -> boolList:
        """Per-cell mask: True where the cell holds a reconstructed interface."""
        ...

    def surface(self) -> reconstructedInterface:
        """Return the reconstructed interface as meshed polygons."""
        ...


def reconstruct(
    mesh: fvMesh,
    alpha: str = "alpha.water",
    scheme: str = "isoAlpha",
) -> reconstructionSchemes:
    """
    Reconstruct the interface from a registered alpha field.

    Looks up ``alpha``, ``phi`` and ``U`` in the mesh registry, builds the
    chosen reconstruction scheme (default ``isoAlpha``) and calls
    :meth:`reconstructionSchemes.reconstruct` once.

    Raises:
        RuntimeError: if ``alpha``, ``phi`` or ``U`` is not registered.
    """
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
