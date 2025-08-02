import pytest
import pandas as pd
import pybFoam
from pybFoam.time_series import Force
import os
import numpy as np
import oftest
from pathlib import Path
from oftest import run_reset_case

@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)

@pytest.mark.skip(reason="post process is not yet ported to cmake")
class TestGroup_postProcess: 

    def test_init(self,run_reset_case):
        log = oftest.path_log()
        assert oftest.case_status(log) == 'completed' # checks if run completes

    def test_postProcess(self,change_test_dir):


        assert Path("postProcessing/pyforce/pyforce.csv").exists()
        py_f = pd.read_csv("postProcessing/pyforce/pyforce.csv")

        f = oftest.read_functionObject("postProcessing/forces/0/force.dat")
        f = f.drop(columns=[4,5,6,7,8,9])
        f.columns = ["t","fx","fy","fz"]

        error = py_f - f
        max_error = max(error.max())
        assert max_error < 1e-6


