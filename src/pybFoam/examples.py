"""Helpers for working with the example cases shipped under ``examples/``.

These utilities exist so that tutorial and how-to scripts in the docs
gallery do not need to repeat case-cloning boilerplate. They locate the
shared ``examples/`` directory and copy a named case into a fresh
temporary directory so that examples are side-effect-free.
"""

from __future__ import annotations

import shutil
import tempfile
from pathlib import Path

__all__ = ["clone_case", "examples_root"]


def examples_root() -> Path:
    """Return the absolute path to the repository's ``examples/`` directory.

    The lookup tries, in order:

    1. ``<repo>/examples`` relative to this module — works in editable
       installs (``pip install -e .``).
    2. Walks up from the current working directory looking for a
       directory named ``examples`` — works when sphinx-gallery sets
       ``cwd`` to the script folder, or when the user runs from a
       checkout.

    Raises
    ------
    FileNotFoundError
        If no ``examples/`` directory can be located.
    """
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

    raise FileNotFoundError(
        "Could not locate the pybFoam examples/ directory. "
        "examples/ ships with the source tree but is not installed "
        "with the wheel — run from a repository checkout or an "
        "editable install (`pip install -e .`)."
    )


def clone_case(name: str, *, prefix: str = "pybfoam_") -> Path:
    """Copy ``examples/<name>/`` into a fresh temporary directory.

    If the source case has a ``0.orig/`` directory, it is restored to
    ``0/`` in the clone (mirroring what an OpenFOAM ``Allrun`` would do
    before launching a solver).

    The polyMesh is *not* generated. Callers run
    ``pybFoam.meshing.generate_blockmesh`` themselves so the meshing
    step stays visible in tutorial code.

    Parameters
    ----------
    name
        Sub-directory under ``examples/`` to clone (e.g. ``"cavity"``).
    prefix
        Prefix for the temporary directory name.

    Returns
    -------
    Path
        Absolute path to the cloned case directory.
    """
    baseline = (examples_root() / name).resolve()
    if not baseline.is_dir():
        raise FileNotFoundError(f"No example case at {baseline}")

    workdir = Path(tempfile.mkdtemp(prefix=prefix))
    target = workdir / name
    shutil.copytree(baseline, target)

    zero, zero_orig = target / "0", target / "0.orig"
    if zero_orig.is_dir():
        if zero.exists():
            shutil.rmtree(zero)
        shutil.copytree(zero_orig, zero)

    return target
