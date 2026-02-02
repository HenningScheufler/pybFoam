"""
Test suite for dimensioned field operations.

Tests dimensionedScalar, dimensionedVector, dimensionedTensor operations
including multiplication, division, dimension arithmetic, and validation.

All dimensioned × field operators are centralized in bind_dimensioned.cpp.
Field × dimensioned operators are in bind_geo_fields.cpp.
"""

import os
from typing import Any, Generator, Tuple

import pytest

import pybFoam


@pytest.fixture(scope="function")
def change_test_dir(request: Any) -> Generator[None, None, None]:
    """Change to test directory for field I/O operations."""
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)


@pytest.fixture(scope="function")
def test_fields(change_test_dir: Any) -> Tuple[Any, Any, Any]:
    """Setup common test fields with known values: scalar=2.0, vector=(1,2,3)."""
    time = pybFoam.Time(".", ".")
    mesh = pybFoam.fvMesh(time)
    p_rgh = pybFoam.volScalarField.read_field(mesh, "p_rgh")
    U = pybFoam.volVectorField.read_field(mesh, "U")
    p_rgh["internalField"] += 2.0
    U["internalField"] += pybFoam.vector(1.0, 2.0, 3.0)
    return mesh, p_rgh, U


# ============================================================================
# Dimensioned Type Creation
# ============================================================================


@pytest.mark.parametrize(
    "dim_type,name,dimension,value,vector_val",
    [
        ("scalar", "nu", pybFoam.dimViscosity, 1e-5, None),
        ("scalar", "p0", pybFoam.dimPressure, 101325.0, None),
        ("scalar", "rho", pybFoam.dimDensity, 1000.0, None),
        ("vector", "U0", pybFoam.dimVelocity, None, pybFoam.vector(1.0, 2.0, 3.0)),
    ],
)
def test_dimensioned_creation(
    dim_type: str, name: str, dimension: Any, value: float, vector_val: Any
) -> None:
    """Test creation of dimensionedScalar and dimensionedVector types."""
    if dim_type == "scalar":
        ds = pybFoam.dimensionedScalar(name, dimension, value)
        assert ds.name() == name
        assert ds.value() == value
    else:
        dv = pybFoam.dimensionedVector(name, dimension, vector_val)
        assert dv.name() == name
        assert dv.value()[0] == vector_val[0]


# ============================================================================
# Operator Tests: dimensioned OP field
# ============================================================================


@pytest.mark.parametrize(
    "op,dim_val,is_vector,expected",
    [
        ("mul", 3.0, False, 6.0),  # 3.0 * 2.0 (scalar)
        ("mul", 2.0, True, 2.0),  # 2.0 * (1,2,3) → check first component
        ("add", 10.0, False, 12.0),  # 10.0 + 2.0
        ("sub", 5.0, False, 3.0),  # 5.0 - 2.0
    ],
)
def test_dimensioned_op_field(
    test_fields: Tuple[Any, Any, Any], op: str, dim_val: float, is_vector: bool, expected: float
) -> None:
    """Test dimensioned × field, dimensioned + field, dimensioned - field."""
    mesh, p_rgh, U = test_fields
    field = U if is_vector else p_rgh
    dim_unit = pybFoam.dimVelocity if is_vector else pybFoam.dimPressure
    dim = pybFoam.dimensionedScalar("dim", dim_unit, dim_val)

    if op == "mul":
        result = dim * field
    elif op == "add":
        result = dim + field
    elif op == "sub":
        result = dim - field

    val = result()["internalField"][0]
    assert (val[0] if is_vector else val) == pytest.approx(expected)


@pytest.mark.parametrize(
    "op,field_scale,dim_val,expected",
    [
        ("mul", 2.0, 3.0, 12.0),  # (2.0 * 2.0) * 3.0
        ("div", 2.0, 2.0, 2.0),  # (2.0 * 2.0) / 2.0
        ("add", 1.0, 5.0, 7.0),  # (2.0 * 1.0) + 5.0
        ("sub", 3.0, 3.0, 3.0),  # (2.0 * 3.0) - 3.0
    ],
)
def test_tmp_field_op_dimensioned(
    test_fields: Tuple[Any, Any, Any], op: str, field_scale: float, dim_val: float, expected: float
) -> None:
    """Test tmp<field> operators with dimensioned (mul, div, add, sub)."""
    mesh, p_rgh, U = test_fields
    tmp_field = p_rgh * field_scale
    dim = pybFoam.dimensionedScalar("dim", pybFoam.dimPressure, dim_val)

    if op == "mul":
        result = tmp_field * dim
    elif op == "div":
        result = tmp_field / dim
    elif op == "add":
        result = tmp_field + dim
    elif op == "sub":
        result = tmp_field - dim

    assert result()["internalField"][0] == pytest.approx(expected)


def test_dimensionedScalar_mul_surfaceScalarField(test_fields: Tuple[Any, Any, Any]) -> None:
    """Test dimensionedScalar × surfaceScalarField."""
    mesh, p_rgh, U = test_fields
    phi = pybFoam.fvc.flux(U)
    scale = pybFoam.dimensionedScalar("scale", pybFoam.dimVelocity * pybFoam.dimArea, 2.0)
    result = scale * phi
    assert result() is not None


# ============================================================================
# DimensionSet Arithmetic
# ============================================================================


@pytest.mark.parametrize(
    "dim1,dim2",
    [
        (pybFoam.dimMass, pybFoam.dimAcceleration),
        (pybFoam.dimVelocity, pybFoam.dimTime),
        (pybFoam.dimPressure, pybFoam.dimArea),
    ],
)
def test_dimensionSet_multiplication(dim1: Any, dim2: Any) -> None:
    """Test dimensionSet × dimensionSet."""
    assert (dim1 * dim2) is not None


@pytest.mark.parametrize(
    "dim1,dim2",
    [
        (pybFoam.dimViscosity, pybFoam.dimDensity),
        (pybFoam.dimVelocity, pybFoam.dimTime),
        (pybFoam.dimForce, pybFoam.dimArea),
    ],
)
def test_dimensionSet_division(dim1: Any, dim2: Any) -> None:
    """Test dimensionSet / dimensionSet."""
    assert (dim1 / dim2) is not None


@pytest.mark.parametrize("dimension,power", [(pybFoam.dimLength, 2), (pybFoam.dimLength, 3)])
def test_dimensionSet_power(dimension: Any, power: int) -> None:
    """Test dimensionSet raised to power."""
    assert (dimension**power) is not None


# ============================================================================
# Physics Pattern Integration Tests
# ============================================================================


def test_pressure_normalization(test_fields: Tuple[Any, Any, Any]) -> None:
    """Test pressure field scaling with reference pressure."""
    mesh, p_rgh, U = test_fields
    p_ref = pybFoam.dimensionedScalar("p_ref", pybFoam.dimPressure, 101325.0)
    p_normalized = p_rgh / p_ref
    assert p_normalized()["internalField"][0] == pytest.approx(2.0 / 101325.0, rel=1e-9)


def test_reynolds_number_dimensions() -> None:
    """Test Reynolds number calculation (dimensionless)."""
    Re_dim = (pybFoam.dimVelocity * pybFoam.dimLength) / pybFoam.dimViscosity
    assert Re_dim is not None


@pytest.mark.parametrize(
    "scale1,scale2,expected",
    [(3.0, 2.0, 3.0), (6.0, 3.0, 4.0)],
)
def test_complex_expression(
    test_fields: Tuple[Any, Any, Any], scale1: float, scale2: float, expected: float
) -> None:
    """Test (dimensioned × field) / dimensioned."""
    mesh, p_rgh, U = test_fields
    ds1 = pybFoam.dimensionedScalar("s1", pybFoam.dimPressure, scale1)
    ds2 = pybFoam.dimensionedScalar("s2", pybFoam.dimPressure, scale2)
    result = (ds1 * p_rgh) / ds2
    assert result()["internalField"][0] == pytest.approx(expected)


# ============================================================================
# Dimension Constants
# ============================================================================


def test_base_dimensions_exist() -> None:
    """Test all SI base dimension constants exist."""
    for dim in [
        "dimless",
        "dimMass",
        "dimLength",
        "dimTime",
        "dimTemperature",
        "dimMoles",
        "dimCurrent",
        "dimLuminousIntensity",
    ]:
        assert hasattr(pybFoam, dim)


def test_derived_dimensions_exist() -> None:
    """Test derived dimension constants exist."""
    for dim in [
        "dimArea",
        "dimVelocity",
        "dimAcceleration",
        "dimForce",
        "dimPressure",
        "dimDensity",
        "dimEnergy",
        "dimPower",
        "dimViscosity",
    ]:
        assert hasattr(pybFoam, dim)
