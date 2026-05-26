"""Tests for ``pybFoam.pyvista_read``."""

from __future__ import annotations

import shutil

import pytest

pytest.importorskip("pyvista")

from pybFoam import (  # noqa: E402
    Time,
    argList,
    clone_example,
    dictionary,
    fvMesh,
    pyvista_read,
    volVectorField,
    write,
)
from pybFoam.meshing import generate_blockmesh  # noqa: E402


def test_pyvista_read_reads_internal_mesh() -> None:
    case = clone_example("cavity")
    try:
        time = Time(argList([str(case), "-case", str(case)]))
        generate_blockmesh(time, dictionary.read(str(case / "system" / "blockMeshDict")))

        mesh = fvMesh(time)
        U = volVectorField.read_field(mesh, "U")
        write(U)

        reader = pyvista_read(case)
        block = reader.read()
        assert "internalMesh" in block.keys()
        internal = block["internalMesh"]
        assert internal is not None
        assert internal.n_cells > 0
        # 100 x 100 x 1 cells in the shipped cavity blockMesh.
        assert internal.n_cells == 100 * 100 * 1
        # The reader should have picked up U from 0/.
        assert "U" in internal.array_names
    finally:
        shutil.rmtree(case.parent, ignore_errors=True)
