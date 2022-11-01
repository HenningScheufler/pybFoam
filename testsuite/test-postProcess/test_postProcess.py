import pytest
import pybFoam
from pybFoam.time_series import Force
import os
import numpy as np
import oftest
from oftest import run_reset_case

@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)

pybFoam.volVectorField.read_field
class TestGroup_postProcess: 

    def test_init(self,run_reset_case):
        log = oftest.path_log()
        assert oftest.case_status(log) == 'completed' # checks if run completes

    def test_postProcess(self,change_test_dir):

        time = pybFoam.Time(".", ".")
        times = pybFoam.selectTimes(time,["test_mesh"])
        mesh = pybFoam.fvMesh(time)

        # f = Force(mesh,["lowerWall"])


        # for idx, t in enumerate(times):
        #     time.setTime(t,idx)
        #     print(t)
        #     p = pybFoam.volScalarField.read_field(mesh,"p")

        #     T = pybFoam.volScalarField.read_field(mesh,"T")

        #     magU = pybFoam.mag(pybFoam.volVectorField.read_field(mesh,"U"))

        #     force = f.compute()
        #     print(force)
    
