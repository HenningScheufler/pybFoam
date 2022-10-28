import os
import oftest
from oftest import run_reset_case
import json

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
