import os
from pathlib import Path
from typing import Any, Generator

import numpy as np
import pytest

import pybFoam

# A closed field has no patch fixing its value (zeroGradient/empty everywhere), so
# OpenFOAM's setRefCell has to pin its level; one fixedValue patch opens it.
CLOSED = ["zeroGradient", "zeroGradient", "zeroGradient", "zeroGradient", "empty"]
OPEN = ["zeroGradient", "zeroGradient", "zeroGradient", "fixedValue", "empty"]


@pytest.fixture(scope="function")
def change_test_dir(request: Any) -> Generator[None, None, None]:
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)


def _field(mesh: pybFoam.fvMesh, name: str, patch_types: list[str]) -> pybFoam.volScalarField:
    value = pybFoam.dimensionedScalar(name, pybFoam.dimPressure, 0.0)
    field = pybFoam.volScalarField.uniform(name, mesh, value, patch_types)
    np.asarray(field["internalField"])[:] = np.arange(mesh.nCells(), dtype=float)
    return field


def _ref_dict(tmp_path: Path, *names: str) -> pybFoam.dictionary:
    entries = "".join(f"{n}RefCell 5;\n{n}RefValue 3.5;\n" for n in names)
    path = tmp_path / "refDict"
    path.write_text(entries)
    return pybFoam.dictionary.read(str(path))


def test_setRefCellInfo_single_field(change_test_dir: Any, tmp_path: Path) -> None:
    time = pybFoam.Time(".", ".")
    mesh = pybFoam.fvMesh(time)
    closed = _field(mesh, "pClosed", CLOSED)
    opened = _field(mesh, "pOpen", OPEN)
    ref_dict = _ref_dict(tmp_path, "pClosed", "pOpen")

    assert pybFoam.setRefCellInfo(closed, ref_dict) == (5, 3.5, True)
    # setRefCell sets the cell to -1 and leaves the value alone when none is needed
    assert pybFoam.setRefCellInfo(opened, ref_dict) == (-1, 0.0, False)
    # forceReference pins even an open field
    assert pybFoam.setRefCellInfo(opened, ref_dict, True) == (5, 3.5, True)
    # the existing two-tuple binding is unchanged
    assert pybFoam.setRefCell(closed, ref_dict) == (5, 3.5)
    assert pybFoam.setRefCell(opened, ref_dict, False) == (-1, 0.0)


def test_setRefCellInfo_two_fields(change_test_dir: Any, tmp_path: Path) -> None:
    # The second field (p_rgh in interFoam) decides; the keys come from the first.
    time = pybFoam.Time(".", ".")
    mesh = pybFoam.fvMesh(time)
    closed = _field(mesh, "pClosed", CLOSED)
    opened = _field(mesh, "pOpen", OPEN)
    ref_dict = _ref_dict(tmp_path, "pOpen")

    assert pybFoam.setRefCellInfo(opened, closed, ref_dict) == (5, 3.5, True)
    assert pybFoam.setRefCellInfo(closed, opened, ref_dict) == (-1, 0.0, False)
    assert pybFoam.setRefCell(opened, closed, ref_dict, False) == (5, 3.5)


def test_getRefCellValue(change_test_dir: Any) -> None:
    time = pybFoam.Time(".", ".")
    mesh = pybFoam.fvMesh(time)
    field = _field(mesh, "pClosed", CLOSED)
    np.asarray(field["internalField"])[:] = 10.0 + 2.0 * np.arange(mesh.nCells())

    assert pybFoam.getRefCellValue(field, 0) == 10.0
    assert pybFoam.getRefCellValue(field, 7) == 24.0
    # a negative cell (no reference / not on this rank) contributes 0
    assert pybFoam.getRefCellValue(field, -1) == 0.0
