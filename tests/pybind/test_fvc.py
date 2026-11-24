import os
from typing import Any, Generator

import numpy as np
import pytest

import pybFoam
from pybFoam import Time, createPhi, fvc, fvMesh, vector, volScalarField, volVectorField


@pytest.fixture(scope="function")
def change_test_dir(request: Any) -> Generator[None, None, None]:
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)


def test_fvc(change_test_dir: Any) -> None:
    # init test case
    time = Time(".", ".")
    mesh = fvMesh(time)
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    U = volVectorField.read_field(mesh, "U")
    phi = createPhi(U)

    grad_p = fvc.grad(p_rgh)()
    assert pybFoam.sum(grad_p["internalField"]) == vector(0, 0, 0)

    div_U = fvc.div(U)()
    assert pybFoam.sum(div_U["internalField"]) == 0

    lap_p = pybFoam.fvc.laplacian(p_rgh)()
    assert pybFoam.sum(lap_p["internalField"]) == 0.0

    phi = pybFoam.fvc.flux(U)()
    div_phiU = pybFoam.fvc.div(phi, U)()
    div_phigradP = pybFoam.fvc.div(phi, pybFoam.fvc.grad(p_rgh))()
    assert pybFoam.sum(div_phiU["internalField"]) == pybFoam.vector(0, 0, 0)
    assert pybFoam.sum(div_phigradP["internalField"]) == pybFoam.vector(0, 0, 0)


def test_fvc_snGrad(change_test_dir: Any) -> None:
    """Test fvc.snGrad for surface normal gradient calculation."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    U = volVectorField.read_field(mesh, "U")

    # Test snGrad on volScalarField
    snGrad_p = fvc.snGrad(p_rgh)
    assert snGrad_p is not None
    # Convert tmp to value if needed
    assert np.all(np.asarray(snGrad_p()["internalField"]) == 0.0)

    # Test snGrad on volVectorField
    snGrad_U = fvc.snGrad(U)
    assert snGrad_U is not None
    assert np.all(np.asarray(snGrad_U()["internalField"]) == [0.0, 0.0, 0.0])


def test_fvc_reconstruct(change_test_dir: Any) -> None:
    """Test fvc.reconstruct for reconstructing vector from flux."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    U = volVectorField.read_field(mesh, "U")

    # Create phi from U
    phi = pybFoam.fvc.flux(U)()

    # Test reconstruct on surfaceScalarField (phi)
    reconstructed = fvc.reconstruct(phi)
    assert reconstructed is not None
    np_recon = np.asarray(reconstructed()["internalField"])
    # Convert tmp to value if needed
    assert np.allclose(np_recon, [0.0, 0.0, 0.0], atol=1e-12)


def test_fvc_div_scalar_convection(change_test_dir: Any) -> None:
    """fvc.div(phi, T) is the Gauss divergence of the convective flux of T."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    U = volVectorField.read_field(mesh, "U")
    T = volScalarField.read_field(mesh, "alpha.water")
    C = np.asarray(mesh.C()["internalField"])
    np.asarray(U["internalField"])[:] = np.column_stack([C[:, 1], -C[:, 0], 0 * C[:, 0]])
    np.asarray(T["internalField"])[:] = 1.0 + C[:, 0] ** 2 + 3.0 * C[:, 1]
    U.correctBoundaryConditions()
    T.correctBoundaryConditions()
    phi = fvc.flux(U)()

    # the default fvSchemes entry div(phi,alpha.water) -> default Gauss linear
    div_T = np.asarray(fvc.div(phi, T)()["internalField"])
    expected = np.asarray(fvc.div(fvc.flux(phi, T))()["internalField"])
    assert np.abs(div_T).max() > 1e-3
    np.testing.assert_allclose(div_T, expected, rtol=1e-12, atol=1e-12)

    # an inline scheme: upwind differs from linear on this rotating flow
    div_up = np.asarray(fvc.div(phi, T, scheme="Gauss upwind")()["internalField"])
    assert np.abs(div_up - div_T).max() > 1e-6
    # ... and the tmp overload gives the same result
    div_tmp = np.asarray(fvc.div(phi, T + T)()["internalField"])
    np.testing.assert_allclose(div_tmp, 2.0 * div_T, rtol=1e-12, atol=1e-12)
