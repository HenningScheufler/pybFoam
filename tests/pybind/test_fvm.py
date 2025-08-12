import pytest
import pybFoam
from pybFoam import fvm, volScalarField, volVectorField, fvMesh, Time, fvScalarMatrix, fvVectorMatrix
import os

@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)

def test_fvm(change_test_dir):

    # init test case
    time = Time(".", ".")
    mesh = fvMesh(time)
    p_rgh = volScalarField.read_field(mesh,"p_rgh")
    U = volVectorField.read_field(mesh,"U")

    lap_p = fvScalarMatrix(fvm.laplacian(p_rgh))
    lap_U = fvVectorMatrix(fvm.laplacian(U))
