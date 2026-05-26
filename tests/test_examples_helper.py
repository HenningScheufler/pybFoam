"""Tests for ``pybFoam.clone_example`` / ``pybFoam.examples_root``."""

from __future__ import annotations

from pathlib import Path

import pytest

from pybFoam import clone_example, examples_root


def test_examples_root_exists() -> None:
    root = examples_root()
    assert root.is_dir()
    assert (root / "cavity").is_dir(), "cavity case should ship under examples/"


def test_clone_example_copies_cavity(tmp_path: Path) -> None:
    case = clone_example("cavity")
    try:
        assert case.is_dir()
        assert (case / "system" / "controlDict").is_file()
        assert (case / "system" / "blockMeshDict").is_file()
        assert (case / "constant" / "transportProperties").is_file()
        # 0.orig must have been restored to 0/.
        assert (case / "0").is_dir()
        assert (case / "0" / "U").is_file()
        assert (case / "0" / "p").is_file()
    finally:
        # Cleanup outside of pytest's tmp_path because clone_example picks
        # its own tempdir.
        import shutil

        shutil.rmtree(case.parent, ignore_errors=True)


def test_clone_example_does_not_mutate_baseline() -> None:
    """Editing the clone must leave the source case untouched."""
    baseline_u = (examples_root() / "cavity" / "0.orig" / "U").read_text()
    case = clone_example("cavity")
    try:
        (case / "0" / "U").write_text("# overwritten")
        assert (examples_root() / "cavity" / "0.orig" / "U").read_text() == baseline_u
    finally:
        import shutil

        shutil.rmtree(case.parent, ignore_errors=True)


def test_clone_example_unknown_name_raises() -> None:
    with pytest.raises(FileNotFoundError):
        clone_example("__definitely_not_an_example__")
