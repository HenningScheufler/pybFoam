import os
import oftest
import pytest
from oftest import run_reset_case
import json

# this is excist as openfoam with an embedded python interpreter can crash if numpy is called
# https://github.com/pybind/pybind11/issues/1889

@pytest.mark.skip(reason="post process is not yet ported to cmake")
def test_numpy(run_reset_case):
    if (not run_reset_case.success):
        oftest.copy_log_files()
    dir_name = oftest.base_dir()
    with open(oftest.path_log()) as log:
        lines = log.readlines()

    for l in lines:
        if "test-numpy-array:" in l:
            assert l.split(":")[1].strip() == "[1 2 3 4 5]"
        if "test-numpy:" in l:
            assert l.split(":")[1].strip() == "Hello"


    assert (run_reset_case.success)
