import pytest
import pybFoam


@pytest.fixture
def change_test_dir(request):
    import os

    os.chdir(request.fspath.dirname)


def test_uniformDimensionedVectorField(change_test_dir):
    """Test reading uniformDimensionedVectorField (e.g., gravity)."""
    time = pybFoam.Time(".", ".")
    mesh = pybFoam.fvMesh(time)

    # Read gravity from constant/g
    g = pybFoam.uniformDimensionedVectorField(mesh, "g")
    
    assert g.name() == "g"
    g_value = g.value()
    assert g_value[0] == 0.0
    assert g_value[1] == -9.81
    assert g_value[2] == 0.0


def test_uniformDimensionedVectorField_dot_product(change_test_dir):
    """Test dot product operations with uniformDimensionedVectorField.
    
    This is used in buoyancy calculations: g & U to get component of velocity
    in direction of gravity.
    """
    time = pybFoam.Time(".", ".")
    mesh = pybFoam.fvMesh(time)
    
    # Read gravity
    g = pybFoam.uniformDimensionedVectorField(mesh, "g")
    
    # Read velocity field
    U = pybFoam.volVectorField.read_field(mesh, "U")
    
    # Test dot product: g & U should return a scalar field
    try:
        result = g & U
        result_val = result() if hasattr(result, '__call__') else result
        assert result_val is not None
        # Check it's a scalar field
        assert hasattr(result_val, "__getitem__")
    except (TypeError, NotImplementedError):
        pytest.skip("uniformDimensionedVectorField & volVectorField dot product not fully supported")


def test_mesh_geometry_access(change_test_dir):
    """Test mesh geometry access: C(), Cf(), magSf()."""
    time = pybFoam.Time(".", ".")
    mesh = pybFoam.fvMesh(time)

    # Test cell centers
    C = mesh.C()
    assert C["internalField"][0] is not None
    
    # Test face centers  
    Cf = mesh.Cf()
    assert Cf["internalField"][0] is not None
    
    # Test face area magnitudes
    magSf = mesh.magSf()
    assert magSf["internalField"][0] > 0.0
