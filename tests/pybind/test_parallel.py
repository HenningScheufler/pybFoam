"""Tests for the global reductions in pybFoam.parallel."""

import os
import shutil
import subprocess
import sys
from pathlib import Path

import numpy as np
import pytest

from pybFoam import parallel, scalarField

WORKER = """
import numpy as np
from pybFoam import Pstream, argList, parallel

args = argList(["parallel_worker", "-parallel"])
assert Pstream.nProcs() == 2

# Process 0 holds no values, process 1 holds all of them
rank = Pstream.myProcNo()
values = np.array([] if rank == 0 else [10.0, 20.0])
weights = np.array([] if rank == 0 else [2.0, 3.0])

assert parallel.sum(weights) == 5.0
assert parallel.min(values) == 10.0
assert parallel.max(values) == 20.0
assert parallel.weighted_average(values, weights) == 16.0

# Reductions must also see values that only exist on process 0
local = np.array([1.0, 7.0] if rank == 0 else [3.0])
assert parallel.sum(local) == 11.0
assert parallel.min(local) == 1.0
assert parallel.max(local) == 7.0
print("WORKER_OK", flush=True)
"""


def test_sum_accepts_arrays_lists_and_fields() -> None:
    assert parallel.sum(np.array([1.0, 2.0])) == 3.0
    assert parallel.sum([1.0, 2.0]) == 3.0
    assert parallel.sum(scalarField([1.0, 2.0])) == 3.0


def test_sum_of_empty_field_is_zero() -> None:
    assert parallel.sum(np.array([])) == 0.0


def test_min_and_max() -> None:
    values = np.array([3.0, -1.0, 2.0])
    assert parallel.min(values) == -1.0
    assert parallel.max(values) == 3.0


@pytest.mark.parametrize("function", [parallel.min, parallel.max])
def test_min_and_max_of_empty_field_raise(function) -> None:  # type: ignore[no-untyped-def]
    with pytest.raises(ValueError, match="empty field"):
        function(np.array([]))


def test_weighted_average() -> None:
    assert parallel.weighted_average([10.0, 20.0], [2.0, 3.0]) == 16.0


def test_weighted_average_rejects_zero_weight() -> None:
    with pytest.raises(ZeroDivisionError):
        parallel.weighted_average([1.0, 2.0], [1.0, -1.0])


def test_weighted_average_rejects_size_mismatch() -> None:
    with pytest.raises(ValueError, match="differ in size"):
        parallel.weighted_average([1.0, 2.0], [1.0])


def test_non_1d_input_raises() -> None:
    with pytest.raises(ValueError, match="1D field"):
        parallel.sum(np.zeros((2, 3)))


@pytest.mark.skipif(shutil.which("mpirun") is None, reason="mpirun not available")
def test_reductions_across_two_processes(tmp_path: Path) -> None:
    system = tmp_path / "system"
    system.mkdir()
    header = "FoamFile { version 2.0; format ascii; class dictionary; object %s; }\n"
    (system / "controlDict").write_text(
        header % "controlDict"
        + "application none; startFrom startTime; startTime 0; stopAt endTime; endTime 1;\n"
        + "deltaT 1; writeControl timeStep; writeInterval 1;\n"
    )
    (system / "decomposeParDict").write_text(
        header % "decomposeParDict" + "numberOfSubdomains 2; method scotch;\n"
    )
    for processor in ("processor0", "processor1"):
        (tmp_path / processor / "0").mkdir(parents=True)

    worker = tmp_path / "worker.py"
    worker.write_text(WORKER)

    env = {**os.environ, "OMPI_MCA_rmaps_base_oversubscribe": "1"}
    result = subprocess.run(
        ["mpirun", "-np", "2", sys.executable, str(worker)],
        cwd=tmp_path,
        env=env,
        capture_output=True,
        text=True,
        timeout=60,
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert result.stdout.count("WORKER_OK") == 2, result.stdout + result.stderr
