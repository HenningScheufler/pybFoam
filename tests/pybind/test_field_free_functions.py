"""
Module-level free functions on Field<T> (mag, magSqr, sqr, sqrt, transcendentals).

These were added when the binding layer was refactored to share generic
helpers; previously these functions were only exposed for vol/surface fields,
not the underlying Field<T> buffer-protocol type.

Parametrized tests cover the family:
  * ``test_mag``/``test_magSqr`` — functions defined for every component type,
    parametrized over the input type.
  * ``test_unary_function_scalar`` — scalar-only functions, parametrized over
    the function.
"""

import math
from typing import Any, Dict, Sequence

import numpy as np
import pytest

import pybFoam as pf

# A small field of each supported component type, keyed by kind.
_FIELDS: Dict[str, Any] = {
    "scalar": pf.scalarField(np.array([1.0, -2.0, 3.0])),
    "vector": pf.vectorField(np.array([[3.0, 4.0, 0.0], [1.0, 2.0, 2.0]])),
    "tensor": pf.tensorField(np.array([[1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]])),
    "symmTensor": pf.symmTensorField(np.array([[1.0, 2.0, 3.0, 4.0, 5.0, 6.0]])),
}


# ---------------------------------------------------------------------------
# mag / magSqr — defined for every component type, return a scalarField
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("kind", ["scalar", "vector", "tensor", "symmTensor"])
def test_mag(kind: str) -> None:
    """``mag`` is bound for every component type and reduces to a non-negative
    scalarField of matching length."""
    f = _FIELDS[kind]
    arr = np.asarray(pf.mag(f)())
    assert arr.ndim == 1
    assert arr.shape[0] == len(f)
    assert np.all(arr >= 0.0)


@pytest.mark.parametrize("kind", ["scalar", "vector", "tensor", "symmTensor"])
def test_magSqr(kind: str) -> None:
    """``magSqr`` is bound for every component type and reduces to a
    non-negative scalarField of matching length."""
    f = _FIELDS[kind]
    arr = np.asarray(pf.magSqr(f)())
    assert arr.ndim == 1
    assert arr.shape[0] == len(f)
    assert np.all(arr >= 0.0)


def test_mag_vector_known_value() -> None:
    """3-4-0 has magnitude 5."""
    vf = pf.vectorField(np.array([[3.0, 4.0, 0.0]]))
    assert np.asarray(pf.mag(vf)())[0] == pytest.approx(5.0)
    assert np.asarray(pf.magSqr(vf)())[0] == pytest.approx(25.0)


# ---------------------------------------------------------------------------
# Scalar-only unary functions — UNARY_FUNCTION family from scalarField.H
# ---------------------------------------------------------------------------


# (func_name, input_values, expected_values)
# Transcendentals are checked against numpy; sign/pos/neg follow OpenFOAM's own
# conventions (sign(0)=1, unlike np.sign) so their expected values are explicit.
_SCALAR_FUNCS = [
    ("sqrt", [0.0, 1.0, 4.0, 9.0], np.sqrt([0.0, 1.0, 4.0, 9.0])),
    ("cbrt", [-8.0, 0.0, 1.0, 27.0], np.cbrt([-8.0, 0.0, 1.0, 27.0])),
    ("exp", [-1.0, 0.0, 1.0, 2.0], np.exp([-1.0, 0.0, 1.0, 2.0])),
    ("log", [1.0, math.e, math.e**2], np.log([1.0, math.e, math.e**2])),
    ("log10", [1.0, 10.0, 100.0], np.log10([1.0, 10.0, 100.0])),
    ("sin", [0.0, math.pi / 2, math.pi], np.sin([0.0, math.pi / 2, math.pi])),
    ("cos", [0.0, math.pi / 2, math.pi], np.cos([0.0, math.pi / 2, math.pi])),
    ("tan", [0.0, math.pi / 4], np.tan([0.0, math.pi / 4])),
    ("sqr", [-3.0, 0.0, 2.0, 5.0], np.square([-3.0, 0.0, 2.0, 5.0])),
    ("sign", [-3.5, 0.0, 2.0], [-1.0, 1.0, 1.0]),
    ("pos", [-1.0, 0.0, 1.0], [0.0, 0.0, 1.0]),
    ("neg", [-1.0, 0.0, 1.0], [1.0, 0.0, 0.0]),
]


@pytest.mark.parametrize("name,values,expected", _SCALAR_FUNCS)
def test_unary_function_scalar(
    name: str, values: Sequence[float], expected: Sequence[float]
) -> None:
    f = pf.scalarField(np.array(values))
    result = np.asarray(getattr(pf, name)(f)())
    assert np.allclose(result, expected, equal_nan=True)


# ---------------------------------------------------------------------------
# Every function must also accept tmp<Field> (the (T, tmp<T>) overload pair)
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("name", ["mag", "magSqr", "sqrt", "exp", "log", "sqr"])
def test_unary_function_accepts_tmp(name: str) -> None:
    """Functions must work on tmp<Field>, not just Field — that's the whole
    point of binding both overloads."""
    f = pf.scalarField(np.array([1.0, 2.0, 3.0]))
    tmp_f = f * 1.0  # produces tmp<scalarField>
    result = getattr(pf, name)(tmp_f)
    assert len(result) == 3
