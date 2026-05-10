"""Tests for ``fvMesh.boundary()`` and the patch accessors.

The iterator binding on ``fvBoundaryMesh`` currently crashes nanobind
with an unrecoverable error when used as ``for p in mesh.boundary()``.
Indexed access via ``len()`` + ``mesh.boundary()[i]`` is the supported
workaround. These tests pin that workaround so we notice if either
shape regresses.
"""

from __future__ import annotations

import shutil
from pathlib import Path
from typing import Tuple

from pybFoam import Time, Word, argList, clone_example, dictionary, fvMesh
from pybFoam.meshing import generate_blockmesh


def _build_cavity() -> Tuple[Path, Time, fvMesh]:
    case = clone_example("cavity")
    time = Time(argList([str(case), "-case", str(case)]))
    generate_blockmesh(time, dictionary.read(str(case / "system" / "blockMeshDict")))
    return case, time, fvMesh(time)


def test_boundary_len_and_index() -> None:
    """``mesh.boundary()`` is len-able and supports integer indexing.

    Indexed access is the workaround for the iterator binding bug, so
    this is the contract that example_blockmesh.py and
    example_modify_boundary_conditions.py rely on.
    """
    case, _time, mesh = _build_cavity()
    try:
        boundary = mesh.boundary()

        n = len(boundary)
        assert n > 0, "cavity should have at least one boundary patch"

        names = []
        sizes = []
        for i in range(n):
            patch = boundary[i]
            names.append(str(patch.name()))
            sizes.append(patch.size())
            assert patch.size() >= 0
            assert patch.start() >= 0

        # The cavity ships these named patches.
        assert "movingWall" in names
        assert "fixedWalls" in names
        # The fv layer collapses ``empty`` patches like frontAndBack to
        # zero size, so we only assert size > 0 on the wall patches.
        for n_name, s in zip(names, sizes):
            if n_name in ("movingWall", "fixedWalls"):
                assert s > 0, f"patch {n_name!r} has zero faces"
    finally:
        shutil.rmtree(case.parent, ignore_errors=True)


def test_boundary_findPatchID() -> None:
    """``findPatchID`` returns the same patch we get via indexed access."""
    case, _time, mesh = _build_cavity()
    try:
        boundary = mesh.boundary()

        idx = boundary.findPatchID(Word("movingWall"))
        assert idx >= 0
        assert str(boundary[idx].name()) == "movingWall"

        # Unknown patches return -1.
        missing = boundary.findPatchID(Word("__no_such_patch__"))
        assert missing == -1
    finally:
        shutil.rmtree(case.parent, ignore_errors=True)
