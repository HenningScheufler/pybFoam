import pytest
import pybFoam
import os
import numpy as np

@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)



def test_geoFieldField(change_test_dir):

    time = pybFoam.Time(".", ".")
    mesh = pybFoam.fvMesh(time)
    p_rgh = pybFoam.volScalarField.read_field(mesh,"p_rgh")

    p_rgh2 = pybFoam.volScalarField(p_rgh) #pybFoam.volScalarField.read_field(mesh,"p_rgh")
    assert pybFoam.sum(p_rgh["internalField"]) == 0
    p_rgh["internalField"] += 1
    assert pybFoam.sum(p_rgh["internalField"]) == len(p_rgh["internalField"])

    print("type ", type(p_rgh2.internalField()))
    np_prgh2 = np.asarray(p_rgh2.internalField())
    np_prgh2 += 1
    assert np_prgh2[0] == 1.0
    assert p_rgh2.internalField()[0] == 1.0
    assert p_rgh2["internalField"][0] == 1.0

    assert pybFoam.sum(p_rgh["leftWall"]) == 0
    p_rgh["leftWall"] += 1
    assert pybFoam.sum(p_rgh["leftWall"]) == len(p_rgh["leftWall"])

    U = pybFoam.volVectorField.read_field(mesh,"U")
    assert (sum(np.array(U["internalField"])) == [0, 0, 0]).all()
    U["internalField"] += pybFoam.vector(1, 1, 1)
    nElements = len(p_rgh["internalField"])
    assert (
        sum(np.array(U["internalField"])) == [nElements, nElements, nElements]
    ).all()

def test_mesh(change_test_dir):

    time = pybFoam.Time(".", ".")
    mesh = pybFoam.fvMesh(time)


    C = mesh.C()
    print(C["internalField"])


def test_mesh(change_test_dir):

    time = pybFoam.Time(".", ".")
    times = pybFoam.selectTimes(time,["test_mesh"])

    for t in times:
        print(t)
    
