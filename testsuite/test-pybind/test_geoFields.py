import pytest
import pybFoam
import os


@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)


def test_geoFieldField(change_test_dir):

    time = pybFoam.Time(".", ".")
    mesh = pybFoam.fvMesh(time)
    p_rgh = pybFoam.volScalarField("p_rgh", mesh)

    p_rgh2 = pybFoam.volScalarField("p_rgh", mesh)
    assert sum(p_rgh["internalField"].to_numpy()) == 0
    p_rgh["internalField"] += 1
    assert sum(p_rgh["internalField"].to_numpy()) == len(p_rgh["internalField"])

    p_rgh2["internalField"] += 1

    test = p_rgh2 + p_rgh

    assert sum(p_rgh["leftWall"].to_numpy()) == 0
    p_rgh["leftWall"] += 1
    assert sum(p_rgh["leftWall"].to_numpy()) == len(p_rgh["leftWall"])

    U = pybFoam.volVectorField("U", mesh)
    assert (sum(U["internalField"].to_numpy()) == [0, 0, 0]).all()
    U["internalField"] += pybFoam.vector(1, 1, 1)
    nElements = len(p_rgh["internalField"])
    assert (
        sum(U["internalField"].to_numpy()) == [nElements, nElements, nElements]
    ).all()

def test_mesh(change_test_dir):

    time = pybFoam.Time(".", ".")
    mesh = pybFoam.fvMesh(time)


    C = mesh.C()
    print(C["internalField"].to_numpy())


def test_mesh(change_test_dir):

    time = pybFoam.Time(".", ".")
    times = pybFoam.selectTimes(time,["test_mesh"])

    for t in times:
        print(t)
    
