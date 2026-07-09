# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 2026 NeoFOAM authors

"""
Type stubs for pybFoam.mules — generic MULES bounded-transport limiter.

MULES (Multidimensional Universal Limiter for Explicit Solution) is not
VoF-specific; these primitives bound any scalar transport to [0, 1] with
zero source terms and can be composed from Python.
"""

from __future__ import annotations

from pybFoam import surfaceScalarField, volScalarField


def explicit_solve(
    psi: volScalarField,
    phi: surfaceScalarField,
    psi_phi: surfaceScalarField,
) -> None:
    """
    MULES explicit solve with unit bounds and zero sources.

    Equivalent to ``MULES::explicitSolve(1, psi, phi, psi_phi, 0, 0, 1, 0)``.
    Updates ``psi`` and ``psi_phi`` in-place.
    """
    ...


def correct(
    psi: volScalarField,
    psi_phi_un: surfaceScalarField,
    psi_phi_corr: surfaceScalarField,
) -> None:
    """
    MULES correction step with unit bounds and zero sources.

    ``psi_phi_corr`` must be initialised to ``(psi_phi_un - psi_phi0)`` before
    the call; afterwards it holds the limited correction flux ready to add to
    ``psi_phi0``.

    Equivalent to ``MULES::correct(1, psi, psi_phi_un, psi_phi_corr, 0, 0, 1, 0)``.
    Updates ``psi`` and ``psi_phi_corr`` in-place.
    """
    ...
