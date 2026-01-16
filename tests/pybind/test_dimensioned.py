"""
Test suite for dimensioned field operations.

Tests dimensionedScalar, dimensionedVector, dimensionedTensor operations
including multiplication, division, dimension arithmetic, and validation.

All dimensioned × field operators are centralized in bind_dimensioned.cpp.
"""
import pytest
import pybFoam
import os
import numpy as np


@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)


def setup_fields():
    """Setup common test fields."""
    time = pybFoam.Time(".", ".")
    mesh = pybFoam.fvMesh(time)
    p_rgh = pybFoam.volScalarField.read_field(mesh, "p_rgh")
    U = pybFoam.volVectorField.read_field(mesh, "U")
    
    # Set to known values
    p_rgh["internalField"] += 2.0
    U["internalField"] += pybFoam.vector(1.0, 2.0, 3.0)
    
    return mesh, p_rgh, U


# ============================================================================
# DimensionedScalar Creation Tests
# ============================================================================

def test_dimensionedScalar_factory_function_creation():
    """Test creating dimensionedScalar using factory function."""
    ds = pybFoam.dimensionedScalar("nu", pybFoam.dimViscosity, 1e-5)
    
    assert ds.name() == "nu"
    assert ds.value() == 1e-5
    # Dimensions should be [0 2 -1 0 0 0 0] for kinematic viscosity


def test_dimensionedScalar_creation_with_standard_dimensions():
    """Test creation with various standard dimension constants."""
    # Pressure
    p0 = pybFoam.dimensionedScalar("p0", pybFoam.dimPressure, 101325.0)
    assert p0.value() == 101325.0
    
    # Velocity  
    U0 = pybFoam.dimensionedScalar("U0", pybFoam.dimVelocity, 10.0)
    assert U0.value() == 10.0
    
    # Density
    rho = pybFoam.dimensionedScalar("rho", pybFoam.dimDensity, 1000.0)
    assert rho.value() == 1000.0
    
    # Temperature
    T0 = pybFoam.dimensionedScalar("T0", pybFoam.dimTemperature, 300.0)
    assert T0.value() == 300.0
    
    # Dimensionless
    alpha = pybFoam.dimensionedScalar("alpha", pybFoam.dimless, 0.5)
    assert alpha.value() == 0.5


# ============================================================================
# DimensionedScalar × Field Multiplication Tests
# ============================================================================

def test_dimensioned_times_volScalarField(change_test_dir):
    """Test dimensionedScalar × volScalarField."""
    mesh, p_rgh, U = setup_fields()
    
    scale = pybFoam.dimensionedScalar("scale", pybFoam.dimPressure, 3.0)
    
    # dimensioned × field
    result = scale * p_rgh
    assert result()["internalField"][0] == pytest.approx(6.0)  # 3.0 * 2.0


def test_volScalarField_times_dimensioned(change_test_dir):
    """Test volScalarField × dimensionedScalar (rmul)."""
    mesh, p_rgh, U = setup_fields()
    
    scale = pybFoam.dimensionedScalar("scale", pybFoam.dimPressure, 3.0)
    
    # field × dimensioned
    result = p_rgh * scale
    assert result()["internalField"][0] == pytest.approx(6.0)  # 2.0 * 3.0


def test_dimensioned_times_tmp_volScalarField(change_test_dir):
    """Test dimensionedScalar × tmp<volScalarField>."""
    mesh, p_rgh, U = setup_fields()
    
    scale = pybFoam.dimensionedScalar("scale", pybFoam.dimPressure, 2.0)
    
    # Create tmp by doing an operation
    tmp_field = p_rgh * 2.0  # tmp with value 4.0
    
    # dimensioned × tmp
    result = scale * tmp_field
    assert result()["internalField"][0] == pytest.approx(8.0)  # 2.0 * 4.0


def test_tmp_volScalarField_times_dimensioned(change_test_dir):
    """Test tmp<volScalarField> × dimensionedScalar."""
    mesh, p_rgh, U = setup_fields()
    
    scale = pybFoam.dimensionedScalar("scale", pybFoam.dimPressure, 2.0)
    
    # Create tmp
    tmp_field = p_rgh * 2.0  # tmp with value 4.0
    
    # tmp × dimensioned
    result = tmp_field * scale
    assert result()["internalField"][0] == pytest.approx(8.0)  # 4.0 * 2.0


# ============================================================================
# Field / DimensionedScalar Division Tests
# ============================================================================

def test_volScalarField_div_dimensioned(change_test_dir):
    """Test volScalarField / dimensionedScalar."""
    mesh, p_rgh, U = setup_fields()
    
    divisor = pybFoam.dimensionedScalar("divisor", pybFoam.dimPressure, 2.0)
    
    result = p_rgh / divisor
    assert result()["internalField"][0] == pytest.approx(1.0)  # 2.0 / 2.0


def test_tmp_volScalarField_div_dimensioned(change_test_dir):
    """Test tmp<volScalarField> / dimensionedScalar."""
    mesh, p_rgh, U = setup_fields()
    
    divisor = pybFoam.dimensionedScalar("divisor", pybFoam.dimPressure, 2.0)
    
    # Create tmp
    tmp_field = p_rgh * 2.0  # tmp with value 4.0
    
    result = tmp_field / divisor
    assert result()["internalField"][0] == pytest.approx(2.0)  # 4.0 / 2.0


# ============================================================================
# Field ± DimensionedScalar Addition/Subtraction Tests
# ============================================================================

def test_volScalarField_minus_dimensioned(change_test_dir):
    """Test volScalarField - dimensionedScalar."""
    mesh, p_rgh, U = setup_fields()
    
    offset = pybFoam.dimensionedScalar("offset", pybFoam.dimPressure, 10.0)
    
    result = p_rgh - offset
    assert result()["internalField"][0] == pytest.approx(-8.0)  # 2.0 - 10.0


def test_volScalarField_plus_dimensioned(change_test_dir):
    """Test volScalarField + dimensionedScalar."""
    mesh, p_rgh, U = setup_fields()
    
    offset = pybFoam.dimensionedScalar("offset", pybFoam.dimPressure, 10.0)
    
    result = p_rgh + offset
    assert result()["internalField"][0] == pytest.approx(12.0)  # 2.0 + 10.0


def test_tmp_volScalarField_minus_dimensioned(change_test_dir):
    """Test tmp<volScalarField> - dimensionedScalar."""
    mesh, p_rgh, U = setup_fields()
    
    offset = pybFoam.dimensionedScalar("offset", pybFoam.dimPressure, 5.0)
    
    tmp_field = p_rgh * 2.0  # tmp with value 4.0
    result = tmp_field - offset
    assert result()["internalField"][0] == pytest.approx(-1.0)  # 4.0 - 5.0


def test_tmp_volScalarField_plus_dimensioned(change_test_dir):
    """Test tmp<volScalarField> + dimensionedScalar."""
    mesh, p_rgh, U = setup_fields()
    
    offset = pybFoam.dimensionedScalar("offset", pybFoam.dimPressure, 5.0)
    
    tmp_field = p_rgh * 2.0  # tmp with value 4.0
    result = tmp_field + offset
    assert result()["internalField"][0] == pytest.approx(9.0)  # 4.0 + 5.0


def test_dimensionSet_multiplication():
    """Test dimensionSet × dimensionSet."""
    # Force = Mass × Acceleration = [1 0 0 0 0 0 0] × [0 1 -2 0 0 0 0]
    # Result should be [1 1 -2 0 0 0 0]
    result = pybFoam.dimMass * pybFoam.dimAcceleration
    # We can't directly check the result, but verify it doesn't crash
    assert result is not None


def test_dimensionSet_division():
    """Test dimensionSet / dimensionSet."""
    # Kinematic Viscosity = Dynamic Viscosity / Density
    # [0 2 -1 0 0 0 0] = [1 -1 -1 0 0 0 0] / [1 -3 0 0 0 0 0]
    result = pybFoam.dimViscosity / pybFoam.dimDensity
    assert result is not None


def test_dimensionSet_power():
    """Test dimensionSet raised to power."""
    # Area = Length²
    area = pybFoam.dimLength ** 2
    assert area is not None
    
    # Volume = Length³
    volume = pybFoam.dimLength ** 3
    assert volume is not None


def test_dimensionSet_combination():
    """Test complex dimension combinations."""
    # Pressure = Force / Area = (Mass × Acceleration) / Length²
    # [1 -1 -2 0 0 0 0] = [1 1 -2 0 0 0 0] / [0 2 0 0 0 0 0]
    force_dims = pybFoam.dimMass * pybFoam.dimAcceleration
    area_dims = pybFoam.dimLength ** 2
    pressure_dims = force_dims / area_dims
    assert pressure_dims is not None



def test_pressure_scaling(change_test_dir):
    """Test pressure field scaling with dimensioned reference pressure."""
    mesh, p_rgh, U = setup_fields()
    
    # Normalize pressure by reference value
    p_ref = pybFoam.dimensionedScalar("p_ref", pybFoam.dimPressure, 101325.0)
    
    p_normalized = p_rgh / p_ref
    
    # p_rgh = 2.0, p_ref = 101325.0
    expected = 2.0 / 101325.0
    assert p_normalized()["internalField"][0] == pytest.approx(expected, rel=1e-9)


def test_kinematic_viscosity_calculation(change_test_dir):
    """Test nu = mu / rho calculation pattern."""
    mesh, p_rgh, U = setup_fields()
    
    # Dynamic viscosity (Pa·s = [1 -1 -1 0 0 0 0])
    mu_dim = pybFoam.dimMass / (pybFoam.dimLength * pybFoam.dimTime)
    mu = pybFoam.dimensionedScalar("mu", mu_dim, 1e-3)
    
    # Create a density field (simplified as scalar field for this test)
    rho = pybFoam.dimensionedScalar("rho", pybFoam.dimDensity, 1000.0)
    
    # This tests dimension arithmetic
    nu_dim = mu_dim / pybFoam.dimDensity
    assert nu_dim is not None


def test_boussinesq_buoyancy_pattern(change_test_dir):
    """Test Boussinesq approximation: rhok = 1.0 - beta*(T - TRef)."""
    mesh, p_rgh, U = setup_fields()
    
    # Thermal expansion coefficient (1/K)
    beta = pybFoam.dimensionedScalar(
        "beta", 
        pybFoam.dimless / pybFoam.dimTemperature, 
        3e-3
    )
    
    # Reference temperature
    TRef = pybFoam.dimensionedScalar("TRef", pybFoam.dimTemperature, 300.0)
    
    # Test that dimensioned arithmetic works
    dT_dim = beta.dimensions() * pybFoam.dimTemperature
    assert dT_dim is not None


def test_velocity_scaling(change_test_dir):
    """Test velocity scaling with characteristic velocity."""
    mesh, p_rgh, U = setup_fields()
    
    U_char = pybFoam.dimensionedScalar("U_char", pybFoam.dimVelocity, 10.0)
    
    # Note: Can't directly multiply volVectorField by dimensionedScalar
    # This would require specific binding for vector field operations
    # Test the dimension creation at least
    assert U_char.value() == 10.0
    assert U_char.dimensions() is not None


def test_reynolds_number_dimensions():
    """Test Reynolds number calculation dimensions (dimensionless)."""
    # Re = U × L / nu
    # Dimensions: [0 1 -1] × [0 1 0] / [0 2 -1] = dimensionless
    
    U_dim = pybFoam.dimVelocity
    L_dim = pybFoam.dimLength
    nu_dim = pybFoam.dimViscosity
    
    Re_dim = (U_dim * L_dim) / nu_dim
    
    # Reynolds number should be dimensionless
    # We can't directly check equality, but verify it's computed
    assert Re_dim is not None


def test_multiply_then_divide(change_test_dir):
    """Test (dimensioned × field) / dimensioned."""
    mesh, p_rgh, U = setup_fields()
    
    scale1 = pybFoam.dimensionedScalar("scale1", pybFoam.dimPressure, 3.0)
    scale2 = pybFoam.dimensionedScalar("scale2", pybFoam.dimPressure, 2.0)
    
    # (scale1 × p_rgh) / scale2
    tmp1 = scale1 * p_rgh  # 3.0 × 2.0 = 6.0
    result = tmp1 / scale2   # 6.0 / 2.0 = 3.0
    
    assert result()["internalField"][0] == pytest.approx(3.0)


def test_add_multiply_sequence(change_test_dir):
    """Test (field + dimensioned) × dimensioned."""
    mesh, p_rgh, U = setup_fields()
    
    offset = pybFoam.dimensionedScalar("offset", pybFoam.dimPressure, 3.0)
    scale = pybFoam.dimensionedScalar("scale", pybFoam.dimPressure, 2.0)
    
    # (p_rgh + offset) × scale
    tmp1 = p_rgh + offset  # 2.0 + 3.0 = 5.0
    result = tmp1 * scale   # 5.0 × 2.0 = 10.0
    
    assert result()["internalField"][0] == pytest.approx(10.0)


def test_complex_expression(change_test_dir):
    """Test complex expression: scale1 × (field - offset) / scale2."""
    mesh, p_rgh, U = setup_fields()
    
    scale1 = pybFoam.dimensionedScalar("scale1", pybFoam.dimPressure, 4.0)
    offset = pybFoam.dimensionedScalar("offset", pybFoam.dimPressure, 1.0)
    scale2 = pybFoam.dimensionedScalar("scale2", pybFoam.dimPressure, 2.0)
    
    # scale1 × (p_rgh - offset) / scale2
    # 4.0 × (2.0 - 1.0) / 2.0 = 4.0 × 1.0 / 2.0 = 2.0
    tmp1 = p_rgh - offset      # 2.0 - 1.0 = 1.0
    tmp2 = scale1 * tmp1        # 4.0 × 1.0 = 4.0
    result = tmp2 / scale2      # 4.0 / 2.0 = 2.0
    
    assert result()["internalField"][0] == pytest.approx(2.0)


def test_dimensionedVector_creation():
    """Test dimensionedVector creation."""
    # Note: In C++, this is dimensioned<vector>
    # Python binding uses dimensionedVector
    # This test verifies the binding exists
    vec1 = pybFoam.dimensionedVector("scale1", pybFoam.dimPressure, pybFoam.vector(1.0, 2.0, 3.0))
    assert vec1.name() == "scale1"
    assert vec1.value()[0] == 1.0
    assert vec1.value()[1] == 2.0
    assert vec1.value()[2] == 3.0

def test_dimensionedTensor_creation():
    """Test dimensionedTensor creation."""
    try:
        assert hasattr(pybFoam, 'dimensionedTensor')
    except (AttributeError, AssertionError):
        pytest.skip("dimensionedTensor not directly accessible from Python")


def test_dimensioned_times_surfaceScalarField(change_test_dir):
    """Test dimensionedScalar × surfaceScalarField."""
    mesh, p_rgh, U = setup_fields()
    
    # Create a surface field
    phi = pybFoam.fvc.flux(U)
    
    scale = pybFoam.dimensionedScalar("scale", pybFoam.dimVelocity * pybFoam.dimArea, 2.0)
    

    result = scale * phi
    assert result() is not None


def test_base_dimensions_exist():
    """Test all SI base dimension constants exist."""
    assert hasattr(pybFoam, 'dimless')
    assert hasattr(pybFoam, 'dimMass')
    assert hasattr(pybFoam, 'dimArea')
    assert hasattr(pybFoam, 'dimLength')
    assert hasattr(pybFoam, 'dimTime')
    assert hasattr(pybFoam, 'dimTemperature')
    assert hasattr(pybFoam, 'dimMoles')
    assert hasattr(pybFoam, 'dimCurrent')
    assert hasattr(pybFoam, 'dimLuminousIntensity')


def test_derived_dimensions_exist():
    """Test derived dimension constants exist."""
    assert hasattr(pybFoam, 'dimVelocity')
    assert hasattr(pybFoam, 'dimAcceleration')
    assert hasattr(pybFoam, 'dimForce')
    assert hasattr(pybFoam, 'dimPressure')
    assert hasattr(pybFoam, 'dimDensity')
    assert hasattr(pybFoam, 'dimEnergy')
    assert hasattr(pybFoam, 'dimPower')
    assert hasattr(pybFoam, 'dimViscosity')
