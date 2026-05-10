"""
Module-level math functions on Field<T> (mag, magSqr, sqr, sqrt, transcendentals).

These were added when the binding layer was refactored to share generic
helpers; previously these functions were only exposed for vol/surface fields,
not the underlying Field<T> buffer-protocol type.
"""

import math
from typing import Any, Callable, Sequence

import numpy as np
import pytest

import pybFoam as pf

# ---------------------------------------------------------------------------
# mag / magSqr — defined for every field type, returns scalarField
# ---------------------------------------------------------------------------


def _make_field(kind: str) -> Any:
    """Construct a small field of each supported component type."""
    if kind == "scalar":
        return pf.scalarField(np.array([1.0, -2.0, 3.0]))
    if kind == "vector":
        return pf.vectorField(np.array([[3.0, 4.0, 0.0], [1.0, 2.0, 2.0]]))
    if kind == "tensor":
        return pf.tensorField(np.array([[1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]]))
    if kind == "symmTensor":
        return pf.symmTensorField(np.array([[1.0, 2.0, 3.0, 4.0, 5.0, 6.0]]))
    raise ValueError(kind)


@pytest.mark.parametrize("kind", ["scalar", "vector", "tensor", "symmTensor"])
def test_mag_returns_scalar_field(kind: str) -> None:
    f = _make_field(kind)
    result = pf.mag(f)
    arr = np.asarray(result())
    assert arr.ndim == 1
    assert arr.shape[0] == len(f)
    assert np.all(arr >= 0.0)


@pytest.mark.parametrize("kind", ["scalar", "vector", "tensor", "symmTensor"])
def test_magSqr_returns_scalar_field(kind: str) -> None:
    f = _make_field(kind)
    result = pf.magSqr(f)
    arr = np.asarray(result())
    assert arr.shape[0] == len(f)
    assert np.all(arr >= 0.0)


def test_mag_vector_known_value() -> None:
    """3-4-0 has magnitude 5."""
    vf = pf.vectorField(np.array([[3.0, 4.0, 0.0]]))
    assert np.asarray(pf.mag(vf)())[0] == pytest.approx(5.0)
    assert np.asarray(pf.magSqr(vf)())[0] == pytest.approx(25.0)


# ---------------------------------------------------------------------------
# Scalar transcendentals — UNARY_FUNCTION family from scalarField.H
# ---------------------------------------------------------------------------


# (pybFoam_func_name, numpy_equivalent, input_values)
_SCALAR_FUNCS = [
    ("sqrt", np.sqrt, [0.0, 1.0, 4.0, 9.0]),
    ("cbrt", np.cbrt, [-8.0, 0.0, 1.0, 27.0]),
    ("exp", np.exp, [-1.0, 0.0, 1.0, 2.0]),
    ("log", np.log, [1.0, math.e, math.e**2]),
    ("log10", np.log10, [1.0, 10.0, 100.0]),
    ("sin", np.sin, [0.0, math.pi / 2, math.pi]),
    ("cos", np.cos, [0.0, math.pi / 2, math.pi]),
    ("tan", np.tan, [0.0, math.pi / 4]),
    ("sqr", np.square, [-3.0, 0.0, 2.0, 5.0]),
]


@pytest.mark.parametrize("name,numpy_fn,values", _SCALAR_FUNCS)
def test_scalar_unary_matches_numpy(
    name: str, numpy_fn: Callable[[np.ndarray], np.ndarray], values: Sequence[float]
) -> None:
    f = pf.scalarField(np.array(values))
    fn = getattr(pf, name)
    result = np.asarray(fn(f)())
    expected = numpy_fn(np.array(values))
    assert np.allclose(result, expected, equal_nan=True)


# sign / pos / neg are integer-valued classifiers. OpenFOAM's `sign(x)` is
# `x >= 0 ? 1 : -1` (returns 1 at zero, unlike numpy's np.sign), so check
# against OpenFOAM's own conventions rather than numpy.
@pytest.mark.parametrize(
    "name,values,expected",
    [
        ("sign", [-3.5, 0.0, 2.0], [-1.0, 1.0, 1.0]),
        ("pos", [-1.0, 0.0, 1.0], [0.0, 0.0, 1.0]),
        ("neg", [-1.0, 0.0, 1.0], [1.0, 0.0, 0.0]),
    ],
)
def test_sign_classifiers(name: str, values: Sequence[float], expected: Sequence[float]) -> None:
    f = pf.scalarField(np.array(values))
    fn = getattr(pf, name)
    result = np.asarray(fn(f)())
    assert np.allclose(result, expected)


# ---------------------------------------------------------------------------
# Each function should also accept tmp<Field> (the (T, tmp<T>) overload pair)
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("name", ["mag", "magSqr", "sqrt", "exp", "log", "sqr"])
def test_accepts_tmp_input(name: str) -> None:
    """Functions must work on tmp<Field>, not just Field — that's the whole
    point of binding both overloads."""
    f = pf.scalarField(np.array([1.0, 2.0, 3.0]))
    tmp_f = f * 1.0  # produces tmp<scalarField>
    fn = getattr(pf, name)
    result = fn(tmp_f)
    assert len(result) == 3
