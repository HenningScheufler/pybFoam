"""
Module-level free functions on vol/surface fields.

Covers the family of functions registered through `bindUnaryFor` after the
binding refactor — `mag`, `magSqr`, `sqr`, `sqrt`, `pow3`, `pow6`, `skew`,
`symm`, `devTwoSymm`, `dev2`, `T` — plus the heterogeneous `max`/`min`/`pow`/
`doubleInner` overloads kept inline.

Time/fvMesh and the fields all live in the same scope (no helper function),
because the fields hold non-owning references back into the mesh — if the
mesh is destroyed first, accessing the fields segfaults.
"""

import os
from typing import Any, Dict, Generator

import numpy as np
import pytest

import pybFoam as pf
from pybFoam import (
    Time,
    fvc,
    fvMesh,
    volScalarField,
    volVectorField,
)


@pytest.fixture(scope="function")
def change_test_dir(request: Any) -> Generator[None, None, None]:
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)


def _make_fields(mesh: Any) -> Dict[str, Any]:
    """Build one field of each component type, all referencing ``mesh``.

    The caller must keep ``mesh`` alive for as long as the returned fields are
    used — the fields hold non-owning references back into it.
    """
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    U = volVectorField.read_field(mesh, "U")
    p_rgh["internalField"] += 4.0
    U["internalField"] += pf.vector(1.0, 2.0, 2.0)
    grad_U = pf.volTensorField(pf.Word("grad_U"), fvc.grad(U))
    sym_gradU = pf.volSymmTensorField(pf.Word("sym_gradU"), pf.symm(grad_U))
    return {"scalar": p_rgh, "vector": U, "tensor": grad_U, "symmTensor": sym_gradU}


# ---------------------------------------------------------------------------
# mag / magSqr — defined across component types, return non-negative scalars
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("kind", ["scalar", "vector", "tensor", "symmTensor"])
def test_mag(change_test_dir: Any, kind: str) -> None:
    """``mag`` is bound for every component type and reduces to a non-negative
    scalar field."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    f = _make_fields(mesh)[kind]
    arr = np.asarray(pf.mag(f)()["internalField"])
    assert arr.ndim == 1
    assert np.all(arr >= 0.0)


# magSqr is not bound for symmTensor vol fields.
@pytest.mark.parametrize("kind", ["scalar", "vector", "tensor"])
def test_magSqr(change_test_dir: Any, kind: str) -> None:
    """``magSqr`` reduces to a non-negative scalar field."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    f = _make_fields(mesh)[kind]
    arr = np.asarray(pf.magSqr(f)()["internalField"])
    assert arr.ndim == 1
    assert np.all(arr >= 0.0)


def test_mag_vector_known_value(change_test_dir: Any) -> None:
    """U set to (1,2,2) ⇒ |U|=3 in every cell, |U|²=9."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    U = volVectorField.read_field(mesh, "U")
    U["internalField"] += pf.vector(1.0, 2.0, 2.0)
    assert np.allclose(np.asarray(pf.mag(U)()["internalField"]), 3.0)
    assert np.allclose(np.asarray(pf.magSqr(U)()["internalField"]), 9.0)


# ---------------------------------------------------------------------------
# Single-type unary functions, parametrized over the function.
#   scalar power/root family — checked against a known value (input is 4.0)
#   tensor decomposition + T + dev2 — smoke tested (expected is None)
# dev2 takes two types, so it appears once per supported input type.
# ---------------------------------------------------------------------------


# (function name, input component type, expected value | None for smoke test)
_UNARY_FUNCS = [
    ("sqrt", "scalar", 2.0),
    ("sqr", "scalar", 16.0),
    ("pow3", "scalar", 64.0),
    ("pow6", "scalar", 4096.0),
    ("skew", "tensor", None),
    ("symm", "tensor", None),
    ("devTwoSymm", "tensor", None),
    ("T", "tensor", None),
    ("dev2", "tensor", None),
    ("dev2", "symmTensor", None),
]


@pytest.mark.parametrize("name,kind,expected", _UNARY_FUNCS)
def test_unary_function(change_test_dir: Any, name: str, kind: str, expected: Any) -> None:
    time = Time(".", ".")
    mesh = fvMesh(time)
    f = _make_fields(mesh)[kind]
    result = getattr(pf, name)(f)
    if expected is None:
        assert result is not None
    else:
        assert np.allclose(np.asarray(result()["internalField"]), expected)


@pytest.mark.parametrize(
    "name,kind",
    [
        ("mag", "tensor"),
        ("magSqr", "tensor"),
        ("sqrt", "scalar"),
        ("sqr", "scalar"),
        ("pow3", "scalar"),
        ("pow6", "scalar"),
        ("skew", "tensor"),
        ("symm", "tensor"),
        ("devTwoSymm", "tensor"),
        ("T", "tensor"),
        ("dev2", "tensor"),
    ],
)
def test_unary_function_accepts_tmp(change_test_dir: Any, name: str, kind: str) -> None:
    """Every unary function must also accept a tmp<...> input (the (T, tmp<T>)
    overload pair). ``fvc.grad(U)`` yields a tmp<volTensorField>; ``f * 1.0``
    yields a tmp<volScalarField>."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    U = volVectorField.read_field(mesh, "U")
    p_rgh["internalField"] += 4.0
    U["internalField"] += pf.vector(1.0, 2.0, 2.0)
    tmp_input = {"scalar": p_rgh * 1.0, "tensor": fvc.grad(U)}[kind]
    assert getattr(pf, name)(tmp_input) is not None


def test_pow_with_exponent(change_test_dir: Any) -> None:
    """pow(field, scalar) — kept inline because the exponent gets wrapped in
    a dimensionedScalar internally."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    p_rgh["internalField"] += 4.0
    assert np.allclose(np.asarray(pf.pow(p_rgh, 0.5)()["internalField"]), 2.0)
    assert np.allclose(np.asarray(pf.pow(p_rgh * 1.0, 0.5)()["internalField"]), 2.0)


# ---------------------------------------------------------------------------
# Heterogeneous: max / min / doubleInner — kept inline because rhs varies
# ---------------------------------------------------------------------------


@pytest.mark.parametrize(
    "name,rhs_kind,expected",
    [
        ("max", "scalar", 4.0),  # max(4, 2) → 4
        ("max", "dimensioned", 4.0),
        ("min", "scalar", 2.0),  # min(4, 2) → 2
        ("min", "dimensioned", 2.0),
    ],
)
def test_max_min_against_scalar(
    change_test_dir: Any, name: str, rhs_kind: str, expected: float
) -> None:
    time = Time(".", ".")
    mesh = fvMesh(time)
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    p_rgh["internalField"] += 4.0

    fn = getattr(pf, name)
    if rhs_kind == "scalar":
        rhs: Any = 2.0
    else:
        # p_rgh has pressure dimensions in this test case
        rhs = pf.dimensionedScalar("two", pf.dimPressure, 2.0)
    assert np.allclose(np.asarray(fn(p_rgh, rhs)()["internalField"]), expected)


def test_max_min_field_vs_field(change_test_dir: Any) -> None:
    time = Time(".", ".")
    mesh = fvMesh(time)
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    p_rgh["internalField"] += 4.0
    g = pf.volScalarField(pf.Word("g"), p_rgh * 0.5)  # value 2 everywhere

    assert np.allclose(np.asarray(pf.max(p_rgh, g)()["internalField"]), 4.0)
    assert np.allclose(np.asarray(pf.min(p_rgh, g)()["internalField"]), 2.0)


def test_doubleInner_tensor_symmTensor(change_test_dir: Any) -> None:
    """doubleInner(volTensorField, volSymmTensorField) → tmp<volScalarField>."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    U = volVectorField.read_field(mesh, "U")
    U["internalField"] += pf.vector(1.0, 2.0, 2.0)
    grad_U = pf.volTensorField(pf.Word("grad_U"), fvc.grad(U))
    sym_gradU = pf.volSymmTensorField(pf.Word("sym_gradU"), pf.symm(grad_U))

    arr = np.asarray(pf.doubleInner(grad_U, sym_gradU)()["internalField"])
    assert arr.ndim == 1
    # rhs may also be tmp<symmTensor>
    assert pf.doubleInner(grad_U, pf.symm(grad_U)) is not None


# ---------------------------------------------------------------------------
# bound — mutating clip
# ---------------------------------------------------------------------------


def test_bound_clips_in_place(change_test_dir: Any) -> None:
    """bound(volScalarField, dimensionedScalar) clamps in place to lower."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    f = pf.volScalarField(p_rgh)  # local copy
    f["internalField"] -= 10.0  # value -10 everywhere
    pf.bound(f, pf.dimensionedScalar("lo", pf.dimPressure, 0.0))
    assert np.all(np.asarray(f["internalField"]) >= -1e-12)
