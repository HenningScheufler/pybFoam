"""Smoke test for ``examples/cavity/icoFoam.py``.

The solver class is shipped as an example, not as library code. This
test exercises the import path tutorial T4 uses and verifies that two
PISO steps actually mutate ``U``.
"""

from __future__ import annotations

import shutil
import sys

import numpy as np
import pytest

from pybFoam import clone_example, dictionary, examples_root


@pytest.fixture(scope="module")
def ico_foam_cls() -> type:
    """Import IcoFoam from examples/cavity/icoFoam.py.

    The same sys.path trick used by tutorial T4. Module-scoped so we
    only insert the path once across this file.
    """
    cavity_dir = str(examples_root() / "cavity")
    if cavity_dir not in sys.path:
        sys.path.insert(0, cavity_dir)
    from icoFoam import IcoFoam  # type: ignore[import-not-found]

    return IcoFoam  # type: ignore[no-any-return]


def test_icofoam_advances_U(ico_foam_cls: type) -> None:
    case = clone_example("cavity")
    try:
        # Truncate runtime to two steps for a fast test.
        cd_path = case / "system" / "controlDict"
        cd = dictionary.read(str(cd_path))
        cd.set("endTime", 0.001)
        cd.set("deltaT", 0.0005)
        cd.set("writeInterval", 0.001)
        cd.write(str(cd_path))

        solver = ico_foam_cls(case)
        u_before = np.asarray(solver.U["internalField"]).copy()

        steps_done = 0
        while solver.step():
            steps_done += 1
        assert steps_done == 2

        u_after = np.asarray(solver.U["internalField"])
        # The lid drives flow into a quiescent cavity; after two steps
        # the interior must have non-zero |U| somewhere.
        assert np.linalg.norm(u_after, axis=1).max() > 0.0
        assert not np.allclose(u_before, u_after)
    finally:
        shutil.rmtree(case.parent, ignore_errors=True)
