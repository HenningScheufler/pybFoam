import pytest
import pybFoam
from pybFoam import fvc, volScalarField, volVectorField, fvMesh, Time, vector, createPhi
import os

@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)

def test_fvc(change_test_dir):

    # init test case
    time = Time(".", ".")
    mesh = fvMesh(time)
    p_rgh = volScalarField.read_field(mesh,"p_rgh")
    U = volVectorField.read_field(mesh,"U")
    phi = createPhi(U)

    grad_p = fvc.grad(p_rgh)()
    assert pybFoam.sum(grad_p["internalField"]) == vector(0,0,0)
    
    div_U = fvc.div(U)()
    assert pybFoam.sum(div_U["internalField"]) == 0

    lap_p = pybFoam.fvc.laplacian(p_rgh)()
    assert pybFoam.sum(lap_p["internalField"]) == 0.0

    phi = pybFoam.fvc.flux(U)()
    div_phiU = pybFoam.fvc.div(phi,U)()
    div_phigradP = pybFoam.fvc.div(phi,pybFoam.fvc.grad(p_rgh))()
    assert pybFoam.sum(div_phiU["internalField"]) == pybFoam.vector(0,0,0)
    assert pybFoam.sum(div_phigradP["internalField"]) == pybFoam.vector(0,0,0)
