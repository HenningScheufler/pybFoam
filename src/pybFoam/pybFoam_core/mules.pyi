"""Generic MULES bounded-transport limiter"""

import pybFoam.pybFoam_core


def explicit_solve(psi: pybFoam.pybFoam_core.volScalarField, phi: pybFoam.pybFoam_core.surfaceScalarField, psi_phi: pybFoam.pybFoam_core.surfaceScalarField) -> None:
    """
    MULES explicit solve with unit bounds and zero sources.
    Equivalent to MULES::explicitSolve(1, psi, phi, psi_phi, 0, 0, 1, 0).
    Updates psi and psi_phi in-place.
    """

def correct(psi: pybFoam.pybFoam_core.volScalarField, psi_phi_un: pybFoam.pybFoam_core.surfaceScalarField, psi_phi_corr: pybFoam.pybFoam_core.surfaceScalarField) -> None:
    """
    MULES correction step with unit bounds and zero sources.
    psi_phi_corr must be initialised to (psi_phi_un - psi_phi0) before the call;
    afterwards it holds the limited correction flux.
    Equivalent to MULES::correct(1, psi, psi_phi_un, psi_phi_corr, 0, 0, 1, 0).
    Updates psi and psi_phi_corr in-place.
    """
