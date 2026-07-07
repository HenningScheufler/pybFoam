import os
from typing import Any, Generator

import pytest

from pybFoam import (
    Time,
    fvc,
    fvm,
    fvMesh,
    fvScalarMatrix,
    fvVectorMatrix,
    volScalarField,
    volVectorField,
)


@pytest.fixture(scope="function")
def change_test_dir(request: Any) -> Generator[None, None, None]:
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)


def test_fvm(change_test_dir: Any) -> None:
    # init test case
    time = Time(".", ".")
    mesh = fvMesh(time)
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    U = volVectorField.read_field(mesh, "U")

    fvScalarMatrix(fvm.laplacian(p_rgh))
    fvVectorMatrix(fvm.laplacian(U))


def test_fvm_div_accepts_tmp_phi(change_test_dir: Any) -> None:
    """fvm.div(flux, F) accepts either surfaceScalarField or
    tmp<surfaceScalarField> as the flux argument."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    U = volVectorField.read_field(mesh, "U")

    phi_tmp = fvc.flux(U)  # tmp<surfaceScalarField>
    phi_concrete = phi_tmp()  # surfaceScalarField (dereferenced)

    fvVectorMatrix(fvm.div(phi_concrete, U))
    fvVectorMatrix(fvm.div(fvc.flux(U), U))
