import pytest
import pybFoam
from pybFoam.time_series import Force
import os
import numpy as np

@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)


def test_postProcess(change_test_dir):

    time = pybFoam.Time(".", ".")
    times = pybFoam.selectTimes(time,["test_mesh"])
    mesh = pybFoam.fvMesh(time)

    f = Force(mesh,["lowerWall"])


    for idx, t in enumerate(times):
        time.setTime(t,idx)
        print(t)
        p_rgh = pybFoam.volScalarField("p_rgh", mesh)

        T = pybFoam.volScalarField("T", mesh)

        magU = pybFoam.mag(pybFoam.volVectorField("U", mesh))

        force = f.compute()
        print(force)
    
