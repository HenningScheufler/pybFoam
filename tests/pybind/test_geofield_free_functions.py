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
from typing import Any, Generator

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


# ---------------------------------------------------------------------------
# mag / magSqr work for every type and return scalar fields
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("kind", ["scalar", "vector", "tensor", "symmTensor"])
def test_mag_returns_scalar_field(change_test_dir: Any, kind: str) -> None:
    """mag is bound for scalar / vector / tensor / symmTensor vol fields."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    U = volVectorField.read_field(mesh, "U")
    p_rgh["internalField"] += 4.0
    U["internalField"] += pf.vector(1.0, 2.0, 2.0)
    grad_U = pf.volTensorField(pf.Word("grad_U"), fvc.grad(U))
    sym_gradU = pf.volSymmTensorField(pf.Word("sym_gradU"), pf.symm(grad_U))

    f: Any = {"scalar": p_rgh, "vector": U, "tensor": grad_U, "symmTensor": sym_gradU}[kind]
    arr = np.asarray(pf.mag(f)()["internalField"])
    assert arr.ndim == 1
    assert np.all(arr >= 0.0)


@pytest.mark.parametrize("kind", ["scalar", "vector", "tensor"])
def test_magSqr_returns_scalar_field(change_test_dir: Any, kind: str) -> None:
    time = Time(".", ".")
    mesh = fvMesh(time)
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    U = volVectorField.read_field(mesh, "U")
    p_rgh["internalField"] += 4.0
    U["internalField"] += pf.vector(1.0, 2.0, 2.0)
    grad_U = pf.volTensorField(pf.Word("grad_U"), fvc.grad(U))

    f: Any = {"scalar": p_rgh, "vector": U, "tensor": grad_U}[kind]
    arr = np.asarray(pf.magSqr(f)()["internalField"])
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
# Scalar power / root family on volScalarField
# ---------------------------------------------------------------------------


# (function name, expected value when input is 4.0)
_SCALAR_FUNCS = [
    ("sqrt", 2.0),
    ("sqr", 16.0),
    ("pow3", 64.0),
    ("pow6", 4096.0),
]


@pytest.mark.parametrize("name,expected", _SCALAR_FUNCS)
def test_scalar_unary_known_values(change_test_dir: Any, name: str, expected: float) -> None:
    time = Time(".", ".")
    mesh = fvMesh(time)
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    p_rgh["internalField"] += 4.0
    fn = getattr(pf, name)
    assert np.allclose(np.asarray(fn(p_rgh)()["internalField"]), expected)


@pytest.mark.parametrize("name", ["sqrt", "sqr", "pow3", "pow6"])
def test_scalar_unary_accepts_tmp(change_test_dir: Any, name: str) -> None:
    """Each function must also accept tmp<volScalarField>."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    p_rgh["internalField"] += 4.0
    fn = getattr(pf, name)
    assert fn(p_rgh * 1.0) is not None


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
# Tensor decomposition functions
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("name", ["skew", "symm", "devTwoSymm"])
def test_tensor_decomposition_runs(change_test_dir: Any, name: str) -> None:
    """Smoke test — these all take volTensorField (concrete or tmp)."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    U = volVectorField.read_field(mesh, "U")
    U["internalField"] += pf.vector(1.0, 2.0, 2.0)
    grad_U = pf.volTensorField(pf.Word("grad_U"), fvc.grad(U))

    fn = getattr(pf, name)
    assert fn(grad_U) is not None
    # tmp<tensor> input
    assert fn(fvc.grad(U)) is not None


def test_T_transpose(change_test_dir: Any) -> None:
    """T(tensor) uses member function .T(); tmp variant uses different code path."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    U = volVectorField.read_field(mesh, "U")
    U["internalField"] += pf.vector(1.0, 2.0, 2.0)
    grad_U = pf.volTensorField(pf.Word("grad_U"), fvc.grad(U))
    assert pf.T(grad_U) is not None
    assert pf.T(fvc.grad(U)) is not None


def test_dev2_works_for_tensor_and_symmTensor(change_test_dir: Any) -> None:
    """dev2 is overloaded for both volTensorField and volSymmTensorField."""
    time = Time(".", ".")
    mesh = fvMesh(time)
    U = volVectorField.read_field(mesh, "U")
    U["internalField"] += pf.vector(1.0, 2.0, 2.0)
    grad_U = pf.volTensorField(pf.Word("grad_U"), fvc.grad(U))
    sym_gradU = pf.volSymmTensorField(pf.Word("sym_gradU"), pf.symm(grad_U))
    assert pf.dev2(grad_U) is not None
    assert pf.dev2(sym_gradU) is not None


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
