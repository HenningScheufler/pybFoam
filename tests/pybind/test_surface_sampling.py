import os
from typing import Any, Generator

import numpy as np
import pytest

from pybFoam import Time, Word, fvMesh, vector, volScalarField, volVectorField
from pybFoam.sampling import (
    SampledCuttingPlaneConfig,
    SampledIsoSurfaceConfig,
    SampledPatchConfig,
    SampledPlaneConfig,
    interpolationScalar,
    interpolationVector,
    sampledSurface,
    sampleOnFacesScalar,
    sampleOnFacesVector,
    sampleOnPointsScalar,
)


@pytest.fixture(scope="function")
def change_test_dir(request: Any) -> Generator[None, None, None]:
    """Change to test directory for OpenFOAM case access."""
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)


def create_time_mesh() -> tuple[Time, fvMesh]:
    """Create OpenFOAM mesh from test case."""
    time = Time(".", ".")
    return time, fvMesh(time)


def test_sampledPlane(change_test_dir: Any) -> None:
    """Test creation, update, and geometry methods of sampledPlane surface."""

    # time needs to be returned to keep alive
    _, mesh = create_time_mesh()
    # Create plane using Pydantic model
    plane_model = SampledPlaneConfig(point=[0.5, 0.5, 0.0], normal=[1.0, 0.0, 0.0])
    plane_dict = plane_model.to_foam_dict()

    # Create plane surface using factory method
    plane = sampledSurface.New(Word("testPlane"), mesh, plane_dict)

    assert plane is not None
    name = plane.name()
    assert name == "testPlane"
    assert plane.enabled()

    # Initially should need update
    assert plane.needsUpdate()

    # Update surface to generate geometry
    result = plane.update()
    assert result  # Should return True on successful update

    # After update, should not need update
    assert not plane.needsUpdate()

    # Test geometry accessors
    points = plane.points()
    Cf = plane.Cf()  # Face centers
    Sf = plane.Sf()  # Face area vectors
    magSf = plane.magSf()  # Face area magnitudes

    assert len(points) > 0
    num_faces = len(magSf)
    assert num_faces > 0
    assert len(Cf) == num_faces
    assert len(Sf) == num_faces

    # Test total area
    area = plane.area()
    assert area > 0

    # Verify area calculation matches sum of face areas
    assert np.isclose(area, np.sum(np.asarray(magSf)), rtol=1e-5)

    # Verify Sf magnitude equals magSf
    Sf_array = np.asarray(Sf)
    magSf_array = np.asarray(magSf)
    Sf_magnitude = np.linalg.norm(Sf_array, axis=1)
    assert np.allclose(Sf_magnitude, magSf_array, rtol=1e-10)


def test_sampledPatch(change_test_dir: Any) -> None:
    """Test creation and geometry of sampledPatch surface."""
    _, mesh = create_time_mesh()

    # Create patch using Pydantic model
    patch_model = SampledPatchConfig(patches=["leftWall"])
    patch_dict = patch_model.to_foam_dict()
    name = Word("testPatch")

    # Create patch surface
    patch_surface = sampledSurface.New(name, mesh, patch_dict)

    # assert patch_surface is not None
    name = patch_surface.name()
    assert name == "testPatch"
    assert patch_surface.enabled()
    assert patch_surface.needsUpdate()

    # Update surface
    assert patch_surface.update()
    assert not patch_surface.needsUpdate()

    # Check geometry
    points = patch_surface.points()
    magSf = patch_surface.magSf()
    assert len(points) > 0
    assert len(magSf) > 0

    # Test geometry accessors
    Cf = patch_surface.Cf()
    Sf = patch_surface.Sf()
    assert len(Cf) == len(magSf)
    assert len(Sf) == len(magSf)

    # Test area
    area = patch_surface.area()
    assert area > 0
    assert np.isclose(area, np.sum(np.asarray(magSf)), rtol=1e-5)


def test_sampledCuttingPlane(change_test_dir: Any) -> None:
    """Test creation and geometry of sampledCuttingPlane surface."""
    _, mesh = create_time_mesh()

    # Create cutting plane using Pydantic model
    cutting_model = SampledCuttingPlaneConfig(point=[0.3, 0.3, 0.0], normal=[1.0, 1.0, 0.0])
    cutting_dict = cutting_model.to_foam_dict()

    # Create cutting plane surface
    cutting_plane = sampledSurface.New(Word("testCuttingPlane"), mesh, cutting_dict)

    assert cutting_plane is not None
    assert cutting_plane.name() == "testCuttingPlane"
    assert cutting_plane.enabled()

    # Update surface
    result = cutting_plane.update()
    assert result
    assert not cutting_plane.needsUpdate()

    # Check geometry
    points = cutting_plane.points()
    magSf = cutting_plane.magSf()
    assert len(points) > 0
    assert len(magSf) > 0

    # Test geometry accessors
    Cf = cutting_plane.Cf()
    Sf = cutting_plane.Sf()
    assert len(Cf) == len(magSf)
    assert len(Sf) == len(magSf)

    # Test area
    area = cutting_plane.area()
    assert area > 0


def test_sampledIsoSurface(change_test_dir: Any) -> None:
    """Test creation and geometry of sampledIsoSurface surface."""
    time, mesh = create_time_mesh()  # time must stay alive for mesh lifetime

    # Read an existing scalar field for the isosurface
    p_rgh = volScalarField.read_field(mesh, "p_rgh")

    # Create a linear pressure distribution in x-direction
    cell_centers = mesh.C()["internalField"]
    p_internal = p_rgh["internalField"]
    for i in range(mesh.nCells()):
        p_internal[i] = cell_centers[i][0]  # x-component

    # Choose isoValue in the middle of the domain
    iso_value = 0.5

    # Create isosurface using Pydantic model
    iso_model = SampledIsoSurfaceConfig(isoField="p_rgh", isoValue=iso_value)
    # Note: 'interpolate' is not a standard isoSurface parameter in the model,
    # but extra fields are allowed, so we can add it to the dict after conversion
    iso_dict = iso_model.to_foam_dict()
    iso_dict.set("interpolate", True)

    # Create isosurface using constructor
    iso_surface = sampledSurface.New(Word("testIsoSurface"), mesh, iso_dict)

    assert iso_surface.name() == "testIsoSurface"
    assert iso_surface is not None
    assert iso_surface.enabled()

    # Update surface
    result = iso_surface.update()
    assert result
    assert not iso_surface.needsUpdate()

    # Check geometry - should have points since we created a linear distribution
    points = iso_surface.points()
    magSf = iso_surface.magSf()
    assert len(points) > 0
    assert len(magSf) > 0

    # Test geometry accessors
    Cf = iso_surface.Cf()
    Sf = iso_surface.Sf()
    assert len(Cf) == len(magSf)
    assert len(Sf) == len(magSf)

    # Test area
    area = iso_surface.area()
    assert area > 0


@pytest.mark.parametrize("scheme", ["cell", "cellPoint", "cellPointFace"])
def test_interpolation_scalar_schemes(change_test_dir: Any, scheme: str) -> None:
    """Test creation of scalar field interpolation schemes and verify interpolated values."""
    time, mesh = create_time_mesh()  # time must stay alive for mesh lifetime

    # Create a scalar field with known linear distribution: f(x,y,z) = x + 2*y + 3*z
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    cell_centers = mesh.C()["internalField"]
    p_internal = p_rgh["internalField"]

    for i in range(mesh.nCells()):
        cc = cell_centers[i]
        p_internal[i] = cc[0] + 2.0 * cc[1] + 3.0 * cc[2]

    # Create a plane at x=0.5 using Pydantic model
    plane_model = SampledPlaneConfig(point=[0.5, 0.5, 0.005], normal=[1.0, 0.0, 0.0])
    plane_dict = plane_model.to_foam_dict()

    plane = sampledSurface.New(Word("testPlane"), mesh, plane_dict)
    plane.update()

    # Test the interpolation scheme
    interp = interpolationScalar.New(Word(scheme), p_rgh)
    assert interp is not None

    # Sample on faces
    sampled = sampleOnFacesScalar(plane, interp)
    assert len(sampled) > 0

    # Get face centers and verify interpolated values
    Cf = plane.Cf()
    sampled_array = np.asarray(sampled)

    # Verify the interpolation is working - check statistics and verify correlation
    assert np.all(np.isfinite(sampled_array)), f"Scheme {scheme}: Non-finite values found"

    # Test that the interpolation shows correlation with the field formula
    # Calculate expected values for all face centers
    expected_values = np.array([fc[0] + 2.0 * fc[1] + 3.0 * fc[2] for fc in Cf])  # type: ignore[attr-defined]

    # Check that the correlation is positive and strong (R^2 > 0.5)
    correlation = np.corrcoef(sampled_array, expected_values)[0, 1]
    assert correlation > 0.7, f"Scheme {scheme}: Poor correlation {correlation}"

    # Check that the mean is in the right ballpark
    expected_mean = np.mean(expected_values)
    sampled_mean = np.mean(sampled_array)
    assert abs(sampled_mean - expected_mean) / expected_mean < 0.5, (
        f"Scheme {scheme}: Mean deviation too large. Expected {expected_mean}, got {sampled_mean}"
    )


@pytest.mark.parametrize("scheme", ["cell", "cellPoint", "cellPointFace"])
def test_interpolation_vector_schemes(change_test_dir: Any, scheme: str) -> None:
    """Test creation of vector field interpolation schemes and verify interpolated values."""
    time, mesh = create_time_mesh()  # time must stay alive for mesh lifetime

    # Create a vector field with known linear distribution
    # U = (x, 2*y, 3*z)
    U = volVectorField.read_field(mesh, "U")
    cell_centers = mesh.C()["internalField"]
    U_internal = U["internalField"]

    for i in range(mesh.nCells()):
        cc = cell_centers[i]
        U_internal[i] = vector(cc[0], 2.0 * cc[1], 3.0 * cc[2])

    # Create a plane at y=0.5 using Pydantic model
    plane_model = SampledPlaneConfig(point=[0.5, 0.5, 0.005], normal=[0.0, 1.0, 0.0])
    plane_dict = plane_model.to_foam_dict()

    plane = sampledSurface.New(Word("testPlane"), mesh, plane_dict)
    plane.update()

    # Test the interpolation scheme
    interp = interpolationVector.New(Word(scheme), U)
    assert interp is not None

    # Sample on faces
    sampled = sampleOnFacesVector(plane, interp)
    assert len(sampled) > 0

    # Verify interpolated values
    sampled_array = np.asarray(sampled)
    assert np.all(np.isfinite(sampled_array)), f"Scheme {scheme}: Non-finite values found"
    assert sampled_array.shape == (len(sampled), 3), f"Scheme {scheme}: Wrong shape"

    # Check that the vector components are in reasonable ranges
    # x component: [0, 1], y component: [0, 2], z component: [0, 0.03]
    assert np.all(sampled_array[:, 0] >= -0.1) and np.all(sampled_array[:, 0] <= 1.1)
    assert np.all(sampled_array[:, 1] >= -0.2) and np.all(sampled_array[:, 1] <= 2.2)
    assert np.all(sampled_array[:, 2] >= -0.01) and np.all(sampled_array[:, 2] <= 0.05)


def test_interpolation_on_points_scalar(change_test_dir: Any) -> None:
    """Test scalar interpolation onto surface points."""
    time, mesh = create_time_mesh()  # time must stay alive for mesh lifetime

    # Create a scalar field with linear distribution: f(x,y,z) = x + y
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    cell_centers = mesh.C()["internalField"]
    p_internal = p_rgh["internalField"]

    for i in range(mesh.nCells()):
        cc = cell_centers[i]
        p_internal[i] = cc[0] + cc[1]

    # Create a plane using Pydantic model
    plane_model = SampledPlaneConfig(point=[0.5, 0.5, 0.005], normal=[0.0, 0.0, 1.0])
    plane_dict = plane_model.to_foam_dict()

    plane = sampledSurface.New(Word("testPlane"), mesh, plane_dict)
    plane.update()

    # Test interpolation to points
    interp = interpolationScalar.New(Word("cellPoint"), p_rgh)
    sampled = sampleOnPointsScalar(plane, interp)

    assert len(sampled) == len(plane.points())

    # Verify all values are finite and in reasonable range
    sampled_array = np.asarray(sampled)
    assert np.all(np.isfinite(sampled_array))
    # Values should be in range [0, 2] for x+y with x,y in [0,1]
    assert np.min(sampled_array) >= -0.2
    assert np.max(sampled_array) <= 2.2


def test_interpolation_on_patch(change_test_dir: Any) -> None:
    """Test interpolation on a patch surface with known boundary values."""
    time, mesh = create_time_mesh()  # time must stay alive for mesh lifetime

    # Create a field with linear distribution: f = x + y
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    cell_centers = mesh.C()["internalField"]
    p_internal = p_rgh["internalField"]

    for i in range(mesh.nCells()):
        cc = cell_centers[i]
        p_internal[i] = cc[0] + cc[1]

    # Create patch surface using Pydantic model
    patch_model = SampledPatchConfig(patches=["leftWall"])
    patch_dict = patch_model.to_foam_dict()

    patch_surface = sampledSurface.New(Word("testPatch"), mesh, patch_dict)
    patch_surface.update()

    # Sample the field
    interp = interpolationScalar.New(Word("cellPoint"), p_rgh)
    sampled = sampleOnFacesScalar(patch_surface, interp)

    # Verify sampling works
    sampled_array = np.asarray(sampled)
    assert len(sampled_array) > 0
    assert np.all(np.isfinite(sampled_array))
    # leftWall is at x=0, so values should be around y (range [0, 1])
    assert np.min(sampled_array) >= -0.1
    assert np.max(sampled_array) <= 1.2


def test_interpolation_scheme_comparison(change_test_dir: Any) -> None:
    """Compare different interpolation schemes on the same field."""
    time, mesh = create_time_mesh()  # time must stay alive for mesh lifetime

    # Create a linear field for easier validation
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    cell_centers = mesh.C()["internalField"]
    p_internal = p_rgh["internalField"]

    for i in range(mesh.nCells()):
        cc = cell_centers[i]
        # Linear function: p = x + y
        p_internal[i] = cc[0] + cc[1]

    # Create a plane using Pydantic model
    plane_model = SampledPlaneConfig(point=[0.5, 0.5, 0.005], normal=[1.0, 0.0, 0.0])
    plane_dict = plane_model.to_foam_dict()

    plane = sampledSurface.New(Word("testPlane"), mesh, plane_dict)
    plane.update()

    # Sample with different schemes
    schemes = ["cell", "cellPoint", "cellPointFace"]
    results = {}

    for scheme in schemes:
        interp = interpolationScalar.New(Word(scheme), p_rgh)
        sampled = sampleOnFacesScalar(plane, interp)
        results[scheme] = np.asarray(sampled)

    # All results should have same length and be finite
    num_faces = len(plane.magSf())
    for scheme, result in results.items():
        assert len(result) == num_faces
        assert np.all(np.isfinite(result))
        # Values should be in range [0.5, 1.5] for x=0.5, y in [0,1]
        # Allow outliers at boundaries
        assert np.min(result) >= 0.1, f"Scheme {scheme}: min too low"
        assert np.max(result) <= 1.7, f"Scheme {scheme}: max too high"
