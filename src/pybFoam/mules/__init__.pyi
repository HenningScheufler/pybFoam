"""MULES bounded-transport limiter (generic)"""

from pybFoam.pybFoam_core import surfaceScalarField, volScalarField


def explicit_solve(
    psi: volScalarField,
    phi: surfaceScalarField,
    psi_phi: surfaceScalarField,
) -> None: ...


def correct(
    psi: volScalarField,
    psi_phi_un: surfaceScalarField,
    psi_phi_corr: surfaceScalarField,
) -> None: ...
