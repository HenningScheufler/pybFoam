import pytest
import pybFoam
import os
import oftest
from oftest import run_reset_case

@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)

class TestGroup: 

    def test_init(self,run_reset_case):
        log = oftest.path_log()
        assert oftest.case_status(log) == 'completed' # checks if run completes
        # assert run_case.success

    def test_fvc(self,change_test_dir):

        # init test case
        time = pybFoam.Time(".", ".")
        mesh = pybFoam.fvMesh(time)
        p_rgh = pybFoam.volScalarField.read_field(mesh,"p_rgh")
        U = pybFoam.volVectorField.read_field(mesh,"U")

        grad_p = pybFoam.fvc.grad(p_rgh).geoField()
        assert pybFoam.sum(grad_p["internalField"]) == pybFoam.vector(0,0,0)
        
        div_U = pybFoam.fvc.div(U).geoField()
        assert pybFoam.sum(div_U["internalField"]) == 0

        lap_p = pybFoam.fvc.laplacian(p_rgh).geoField()
        assert pybFoam.sum(lap_p["internalField"]) == 0.0

        phi = pybFoam.fvc.flux(U).geoField()
        div_phiU = pybFoam.fvc.div(phi,U).geoField()
        div_phigradP = pybFoam.fvc.div(phi,pybFoam.fvc.grad(p_rgh)).geoField()
        assert pybFoam.sum(div_phiU["internalField"]) == pybFoam.vector(0,0,0)
        assert pybFoam.sum(div_phigradP["internalField"]) == pybFoam.vector(0,0,0)
