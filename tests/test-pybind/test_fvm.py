import pytest
import pybFoam
from pybFoam import fvm, volScalarField, volVectorField, fvMesh, Time, fvScalarMatrix, fvVectorMatrix
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

    def test_fvm(self,change_test_dir):

        # init test case
        time = Time(".", ".")
        mesh = fvMesh(time)
        p_rgh = volScalarField.read_field(mesh,"p_rgh")
        U = volVectorField.read_field(mesh,"U")

        lap_p = fvScalarMatrix(fvm.laplacian(p_rgh))
        lap_U = fvVectorMatrix(fvm.laplacian(U))
