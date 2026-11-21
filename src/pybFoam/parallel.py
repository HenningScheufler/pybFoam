"""Global reductions over all MPI processes.

Import the module and keep the namespace, so that global operations stay
distinct from Python's and NumPy's local ``sum``, ``min`` and ``max``::

    from pybFoam import parallel

    mass_flow = parallel.sum(phi)
    T_mixed = parallel.weighted_average(T, phi)

Every function is collective: all processes must call it in the same order,
including processes that hold no values.
"""

from __future__ import annotations

import numpy as np
from numpy.typing import ArrayLike, NDArray

from .pybFoam_core import gMax, gMin, scalarField
from .pybFoam_core import sum as _gSum

__all__ = ["max", "min", "sum", "weighted_average"]

ScalarValues = ArrayLike | scalarField


def _as_array(values: ScalarValues) -> NDArray[np.float64]:
    array = np.asarray(values, dtype=np.float64)
    if array.ndim != 1:
        raise ValueError(f"Expected a 1D field, got shape {array.shape}")
    return array


def _global_size(array: NDArray[np.float64]) -> int:
    return int(_gSum(scalarField(np.ones(array.size))))


def sum(values: ScalarValues) -> float:
    """Return the sum over all processes."""
    return float(_gSum(scalarField(_as_array(values))))


def min(values: ScalarValues) -> float:
    """Return the minimum over all processes."""
    array = _as_array(values)
    if _global_size(array) == 0:
        raise ValueError("parallel.min() of an empty field")
    return float(gMin(scalarField(array)))


def max(values: ScalarValues) -> float:
    """Return the maximum over all processes."""
    array = _as_array(values)
    if _global_size(array) == 0:
        raise ValueError("parallel.max() of an empty field")
    return float(gMax(scalarField(array)))


def weighted_average(values: ScalarValues, weights: ScalarValues) -> float:
    """Return ``sum(values * weights) / sum(weights)`` over all processes."""
    value_array = _as_array(values)
    weight_array = _as_array(weights)
    if value_array.size != weight_array.size:
        raise ValueError(
            f"values and weights differ in size: {value_array.size} and {weight_array.size}"
        )

    total_weight = sum(weight_array)
    if total_weight == 0:
        raise ZeroDivisionError("Sum of weights is zero")
    return sum(value_array * weight_array) / total_weight
