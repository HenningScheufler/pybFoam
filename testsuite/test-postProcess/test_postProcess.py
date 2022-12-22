import pytest
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


class TestGroup_postProcess: 

    def test_init(self,run_reset_case):
        log = oftest.path_log()
        assert oftest.case_status(log) == 'completed' # checks if run completes

    def test_postProcess(self,change_test_dir):

        assert Path("postProcessing/pyforce/pyforce.csv").exists()

    
