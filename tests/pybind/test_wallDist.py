import os
from typing import Any, Generator

import numpy as np
import pytest

import pybFoam


@pytest.fixture(scope="function")
def change_test_dir(request: Any) -> Generator[None, None, None]:
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)


def test_wallDist_import() -> None:
    from pybFoam import wallDist

    assert wallDist is not None


def test_wallDist_y(change_test_dir: Any) -> None:
    time = pybFoam.Time(".", ".")
    mesh = pybFoam.fvMesh(time)
    wd = pybFoam.wallDist.New(mesh)

    y = wd.y()
    assert y is not None

    y_internal = np.asarray(y["internalField"])
    assert len(y_internal) == mesh.nCells()
    assert np.all(y_internal >= 0.0)


def test_nearWallDist(change_test_dir: Any) -> None:
    time = pybFoam.Time(".", ".")
    mesh = pybFoam.fvMesh(time)
    nwd = pybFoam.nearWallDist(mesh)
    nwd.correct()


def test_nearWallDistNoSearch(change_test_dir: Any) -> None:
    time = pybFoam.Time(".", ".")
    mesh = pybFoam.fvMesh(time)
    nwd = pybFoam.nearWallDistNoSearch(mesh)
    nwd.correct()
