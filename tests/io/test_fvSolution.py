from typing import Optional
import pytest
from pybFoam.io.model_base import IOModelBase
import os


@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)


class MatrixSolver(IOModelBase):
    solver: str
    preconditioner: Optional[str] = None
    smoother: Optional[str] = None  # Optional, not always present
    tolerance: float
    relTol: float


class Solvers(IOModelBase):
    p: MatrixSolver
    pFinal: MatrixSolver
    U: MatrixSolver


class PISO(IOModelBase):
    nCorrectors: int
    nNonOrthogonalCorrectors: int


class FvSolution(IOModelBase):
    solvers: Solvers
    PISO: PISO


def test_parse_fvSolution(change_test_dir):
    model = FvSolution.from_file("fvSolution")

    assert model.solvers.p.solver == "PCG"
    assert model.solvers.p.preconditioner == "DIC"
    assert model.solvers.p.tolerance == 1e-06
    assert model.solvers.p.relTol == 0.05

    assert model.solvers.pFinal.solver == "PCG"
    assert model.solvers.pFinal.preconditioner == "DIC"
    assert model.solvers.pFinal.tolerance == 1e-06
    assert model.solvers.pFinal.relTol == 0.00

    assert model.solvers.U.solver == "smoothSolver"
    assert model.solvers.U.smoother == "symGaussSeidel"
    assert model.solvers.U.preconditioner is None
    assert model.solvers.U.tolerance == 1e-05
    assert model.solvers.U.relTol == 0.00

    assert model.PISO.nCorrectors == 2
    assert model.PISO.nNonOrthogonalCorrectors == 2
