"""Helpers for the example cases shipped under ``examples/``."""

from __future__ import annotations

import shutil
import tempfile
from pathlib import Path
from typing import TYPE_CHECKING, Iterable, Optional, Union

if TYPE_CHECKING:
    import pyvista as pv

__all__ = ["clone_example", "examples_root", "pyvista_read"]


def examples_root() -> Path:
    """Path to the repo's ``examples/`` directory."""
    pkg_candidate = Path(__file__).resolve().parents[2] / "examples"
    if pkg_candidate.is_dir():
        return pkg_candidate

    cwd = Path.cwd().resolve()
    for parent in [cwd, *cwd.parents]:
        candidate = parent / "examples"
        if candidate.is_dir():
            return candidate
        if parent.name == "examples" and parent.is_dir():
            return parent

    raise FileNotFoundError("examples/ not found — run from a repo checkout or `pip install -e .`.")


def clone_example(name: str, *, prefix: str = "pybfoam_") -> Path:
    """Copy ``examples/<name>/`` to a tmp dir, restoring ``0.orig`` → ``0``."""
    baseline = (examples_root() / name).resolve()
    if not baseline.is_dir():
        raise FileNotFoundError(f"No example case at {baseline}")

    target = Path(tempfile.mkdtemp(prefix=prefix)) / name
    shutil.copytree(baseline, target)

    zero, zero_orig = target / "0", target / "0.orig"
    if zero_orig.is_dir():
        if zero.exists():
            shutil.rmtree(zero)
        shutil.copytree(zero_orig, zero)

    return target


def pyvista_read(
    case: Union[str, Path],
    *,
    time: Optional[float] = None,
    cell_arrays: Optional[Iterable[str]] = None,
) -> "pv.POpenFOAMReader":
    """Read an OpenFOAM case via pyvista (fields must be on disk)."""
    try:
        import pyvista as pv
    except ImportError as exc:
        raise ImportError("pyvista is required; `pip install pybFoam[docs]`.") from exc

    case_path = Path(case).resolve()
    if not case_path.is_dir():
        raise FileNotFoundError(f"Case directory does not exist: {case_path}")

    marker = case_path / f"{case_path.name}.foam"
    if not marker.exists():
        marker.touch()

    reader = pv.POpenFOAMReader(str(marker))

    if cell_arrays is None:
        reader.enable_all_cell_arrays()
    else:
        reader.disable_all_cell_arrays()
        for name in cell_arrays:
            reader.enable_cell_array(name)

    if time is not None:
        reader.set_active_time_value(time)  # type: ignore[no-untyped-call,unused-ignore]

    return reader
