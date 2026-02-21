import os
from typing import Any, Generator

import numpy as np
import pytest

from pybFoam import (
    Time,
    Word,
    fvMesh,
    vector,
    volScalarField,
    volVectorField,
)
from pybFoam.sampling import (
    interpolationScalar,
    interpolationVector,
    meshSearch,
    sampledSet,
    sampleSetScalar,
    sampleSetVector,
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


def test_meshSearch_creation(change_test_dir: Any) -> None:
    """Test creation of meshSearch object."""
    _, mesh = create_time_mesh()

    # Create meshSearch from mesh
    search = meshSearch(mesh)

    assert search is not None


def test_meshSearch_find_cell(change_test_dir: Any) -> None:
    """Test finding cells using meshSearch."""
    _, mesh = create_time_mesh()
    search = meshSearch(mesh)

    # Find cell at center of domain
    center_point = vector(0.5, 0.5, 0.005)
    cell_id = search.findCell(center_point, -1, True)

    # Should find a valid cell
    assert cell_id >= 0
    assert cell_id < mesh.nCells()


def test_sampledSet_uniform_line(change_test_dir: Any) -> None:
    """Test creation of uniform line sampledSet."""
    from pybFoam.sampling import UniformSetConfig

    _, mesh = create_time_mesh()
    search = meshSearch(mesh)

    # Create uniform line using Pydantic config
    config = UniformSetConfig(
        axis="distance", start=[0.1, 0.5, 0.005], end=[0.55, 0.5, 0.005], nPoints=50
    )
    set_dict = config.to_foam_dict()

    # Create sampledSet
    line = sampledSet.New(Word("testLine"), mesh, search, set_dict)

    assert line is not None
    assert line.name() == "testLine"
    assert line.axis() == "distance"

    # Check points
    points = line.points()
    assert len(points) > 0
    assert len(points) <= 50  # May be fewer if some points are outside mesh

    # Check distance
    distance = line.distance()
    assert len(distance) == len(points)
    assert distance[0] == pytest.approx(0.0, abs=1e-6)
    # Distance should increase monotonically
    for i in range(1, len(distance)):
        assert distance[i] > distance[i - 1]

    # Check cells and faces
    cells = line.cells()
    assert len(cells) == len(points)

    # Most cells should be valid (>= 0)
    valid_cells = sum(1 for c in cells if c >= 0)
    assert valid_cells > len(points) * 0.7  # At least 70% valid


@pytest.mark.parametrize("axis_name", ["x", "y", "z", "xyz", "distance"])
def test_sampledSet_uniform_axes(change_test_dir: Any, axis_name: str) -> None:
    """Test uniform sampledSet with different axis specifications."""
    from pybFoam.sampling import UniformSetConfig

    _, mesh = create_time_mesh()
    search = meshSearch(mesh)

    # Create uniform line using Pydantic config
    config = UniformSetConfig(
        axis=axis_name, start=[0.1, 0.1, 0.005], end=[0.55, 0.55, 0.005], nPoints=20
    )
    set_dict = config.to_foam_dict()

    line = sampledSet.New(Word(f"line_{axis_name}"), mesh, search, set_dict)

    assert line is not None
    assert line.axis() == axis_name
    # May have fewer points if some are outside mesh
    assert line.nPoints() > 0
    assert line.nPoints() <= 20


def test_sampledSet_cloud(change_test_dir: Any) -> None:
    """Test creation of cloud (arbitrary points) sampledSet."""
    from pybFoam.sampling import CloudSetConfig

    _, mesh = create_time_mesh()
    search = meshSearch(mesh)

    # Create cloud set using Pydantic config
    # Domain is [0, 0.584] x [0, 0.584] x [0, 0.0146]
    config = CloudSetConfig(
        axis="xyz", points=[[0.1, 0.1, 0.005], [0.3, 0.3, 0.005], [0.5, 0.5, 0.005]]
    )
    set_dict = config.to_foam_dict()

    # Create sampledSet
    cloud = sampledSet.New(Word("testCloud"), mesh, search, set_dict)

    assert cloud is not None
    assert cloud.name() == "testCloud"
    # All 3 points should be valid within domain bounds
    assert cloud.nPoints() == 3

    points = cloud.points()
    assert len(points) == 3

    # Verify all three points are present
    assert np.allclose(np.array(points[0]), [0.1, 0.1, 0.005], atol=1e-6)
    assert np.allclose(np.array(points[1]), [0.3, 0.3, 0.005], atol=1e-6)
    assert np.allclose(np.array(points[2]), [0.5, 0.5, 0.005], atol=1e-6)


def test_sampledSet_polyLine(change_test_dir: Any) -> None:
    """Test creation of polyLine (multi-segment) sampledSet."""
    from pybFoam.sampling import PolyLineSetConfig

    _, mesh = create_time_mesh()
    search = meshSearch(mesh)

    # Create polyline set using Pydantic config
    # Domain is [0, 0.584] x [0, 0.584] x [0, 0.0146]
    config = PolyLineSetConfig(
        axis="distance",
        points=[[0.1, 0.1, 0.005], [0.55, 0.1, 0.005], [0.55, 0.55, 0.005], [0.1, 0.55, 0.005]],
        nPoints=60,
    )
    set_dict = config.to_foam_dict()

    # Create sampledSet
    polyline = sampledSet.New(Word("testPolyLine"), mesh, search, set_dict)

    assert polyline is not None
    assert polyline.name() == "testPolyLine"
    # May have fewer points if some are outside mesh
    assert polyline.nPoints() > 0
    assert polyline.nPoints() <= 60

    # Check that segments are tracked
    segments = polyline.segments()
    assert len(segments) == polyline.nPoints()

    # Distance should be monotonically increasing
    distance = polyline.distance()
    if len(distance) > 1:
        for i in range(len(distance) - 1):
            assert distance[i + 1] >= distance[i]


def test_sampledSet_circle(change_test_dir: Any) -> None:
    """Test creation of circle sampledSet."""
    from pybFoam.sampling import CircleSetConfig

    _, mesh = create_time_mesh()
    search = meshSearch(mesh)

    # Create circle set using Pydantic config
    # Domain is [0, 0.584] x [0, 0.584], so center at 0.292 with radius 0.08
    config = CircleSetConfig(
        axis="distance",
        origin=[0.292, 0.292, 0.005],
        circleAxis=[0.0, 0.0, 1.0],
        startPoint=[0.372, 0.292, 0.005],  # Radius 0.08
        dTheta=10.0,  # 10 degree increments
    )
    set_dict = config.to_foam_dict()

    # Create sampledSet
    circle = sampledSet.New(Word("testCircle"), mesh, search, set_dict)

    assert circle is not None
    assert circle.name() == "testCircle"

    # Some points may be outside mesh, so check for reasonable number
    assert circle.nPoints() > 0
    assert circle.nPoints() <= 36  # Max 360/10 = 36 points

    points = circle.points()
    assert len(points) > 0


def test_sample_scalar_field_on_uniform_set(change_test_dir: Any) -> None:
    """Test sampling scalar field onto uniform line set."""
    time, mesh = create_time_mesh()  # time must stay alive for mesh lifetime
    search = meshSearch(mesh)

    # Create a scalar field with linear distribution: f(x,y,z) = x + 2*y
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    cell_centers = mesh.C()["internalField"]
    p_internal = p_rgh["internalField"]

    for i in range(mesh.nCells()):
        cc = cell_centers[i]
        p_internal[i] = cc[0] + 2.0 * cc[1]

    # Create uniform line along x at y=0.292 (center of domain) using Pydantic config
    from pybFoam.sampling import UniformSetConfig

    config = UniformSetConfig(
        axis="x", start=[0.05, 0.292, 0.005], end=[0.55, 0.292, 0.005], nPoints=30
    )
    set_dict = config.to_foam_dict()

    line = sampledSet.New(Word("xLine"), mesh, search, set_dict)

    # Create interpolation and sample
    interp = interpolationScalar.New(Word("cellPoint"), p_rgh)
    sampled = sampleSetScalar(line, interp)

    # Check we got samples
    assert len(sampled) > 0
    assert len(sampled) <= 30

    # Verify sampled values
    sampled_array = np.asarray(sampled)
    points = line.points()

    # Filter out invalid points (marked with max value)
    valid_mask = sampled_array < 1e10
    valid_samples = sampled_array[valid_mask]
    valid_points = [points[i] for i in range(len(points)) if valid_mask[i]]

    assert len(valid_samples) > len(sampled) * 0.5  # At least 50% valid

    # Expected value at y=0.292: f = x + 2*0.292 = x + 0.584
    # So values should range from approximately 0.634 (0.05+0.584) to 1.134 (0.55+0.584)
    expected_values = np.array([pt[0] + 0.584 for pt in valid_points])

    # Check correlation
    if len(valid_samples) > 2:
        correlation = np.corrcoef(valid_samples, expected_values)[0, 1]
        assert correlation > 0.8


def test_sample_vector_field_on_uniform_set(change_test_dir: Any) -> None:
    """Test sampling vector field onto uniform line set."""
    time, mesh = create_time_mesh()  # time must stay alive for mesh lifetime
    search = meshSearch(mesh)

    # Create a vector field: U = (x, 2*y, 0)
    U = volVectorField.read_field(mesh, "U")
    cell_centers = mesh.C()["internalField"]
    U_internal = U["internalField"]

    for i in range(mesh.nCells()):
        cc = cell_centers[i]
        U_internal[i] = vector(cc[0], 2.0 * cc[1], 0.0)

    # Create diagonal line using Pydantic config
    from pybFoam.sampling import UniformSetConfig

    config = UniformSetConfig(
        axis="distance", start=[0.1, 0.1, 0.005], end=[0.55, 0.55, 0.005], nPoints=25
    )
    set_dict = config.to_foam_dict()

    line = sampledSet.New(Word("diagLine"), mesh, search, set_dict)

    # Create interpolation and sample
    interp = interpolationVector.New(Word("cellPoint"), U)
    sampled = sampleSetVector(line, interp)

    # Check we got samples
    assert len(sampled) > 0
    assert len(sampled) <= 25

    # Verify sampled values
    sampled_array = np.asarray(sampled)
    assert sampled_array.shape[1] == 3

    # Filter out invalid points
    valid_mask = np.all(sampled_array < 1e10, axis=1)
    valid_samples = sampled_array[valid_mask]

    assert len(valid_samples) > len(sampled) * 0.5

    # Check that components are in reasonable ranges
    # Line goes from (0.1, 0.1) to (0.55, 0.55)
    if len(valid_samples) > 0:
        assert np.all(valid_samples[:, 0] >= 0.05) and np.all(valid_samples[:, 0] <= 0.58)
        assert np.all(valid_samples[:, 1] >= 0.15) and np.all(valid_samples[:, 1] <= 1.15)
        assert np.all(np.abs(valid_samples[:, 2]) < 0.1)  # z component should be near zero


def test_sample_on_cloud_set(change_test_dir: Any) -> None:
    """Test sampling on cloud (arbitrary points) set."""
    time, mesh = create_time_mesh()  # time must stay alive for mesh lifetime
    search = meshSearch(mesh)

    # Create simple field: f = x + y
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    cell_centers = mesh.C()["internalField"]
    p_internal = p_rgh["internalField"]

    for i in range(mesh.nCells()):
        cc = cell_centers[i]
        p_internal[i] = cc[0] + cc[1]

    # Create cloud with specific points using Pydantic config
    from pybFoam.sampling import CloudSetConfig

    config = CloudSetConfig(
        axis="xyz", points=[[0.2, 0.2, 0.005], [0.35, 0.35, 0.005], [0.5, 0.5, 0.005]]
    )
    set_dict = config.to_foam_dict()

    cloud = sampledSet.New(Word("testCloud"), mesh, search, set_dict)

    # Sample the field
    interp = interpolationScalar.New(Word("cell"), p_rgh)
    sampled = sampleSetScalar(cloud, interp)

    sampled_array = np.asarray(sampled)

    # Filter valid samples
    valid_mask = sampled_array < 1e10
    valid_samples = sampled_array[valid_mask]

    # Should have at least some valid points
    assert len(valid_samples) > 0

    # Expected values around [0.4, 0.7, 1.0] for points [(0.2,0.2), (0.35,0.35), (0.5,0.5)]
    # f = x + y, so [0.4, 0.7, 1.0] but may vary with interpolation
    assert np.all(valid_samples >= 0.3)
    assert np.all(valid_samples <= 1.1)


@pytest.mark.parametrize("scheme", ["cell", "cellPoint", "cellPointFace"])
def test_multiple_sampling_schemes(change_test_dir: Any, scheme: str) -> None:
    """Test different interpolation schemes on sampledSet."""
    time, mesh = create_time_mesh()  # time must stay alive for mesh lifetime
    search = meshSearch(mesh)

    # Create linear field
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    cell_centers = mesh.C()["internalField"]
    p_internal = p_rgh["internalField"]

    for i in range(mesh.nCells()):
        cc = cell_centers[i]
        p_internal[i] = 2.0 * cc[0] + cc[1]

    # Create uniform line using Pydantic config
    from pybFoam.sampling import UniformSetConfig

    config = UniformSetConfig(
        axis="distance", start=[0.1, 0.15, 0.005], end=[0.5, 0.45, 0.005], nPoints=20
    )
    set_dict = config.to_foam_dict()

    line = sampledSet.New(Word("testLine"), mesh, search, set_dict)

    # Test the interpolation scheme
    interp = interpolationScalar.New(Word(scheme), p_rgh)
    sampled = sampleSetScalar(line, interp)
    sampled_array = np.asarray(sampled)

    # Filter valid samples
    valid_mask = sampled_array < 1e10
    values = sampled_array[valid_mask]

    # Should produce some valid samples
    assert len(values) > 0
    assert np.all(np.isfinite(values))
    # Line goes from (0.1, 0.15) to (0.5, 0.45)
    # f = 2*x + y, so range: [2*0.1+0.15, 2*0.5+0.45] = [0.35, 1.45]
    assert np.min(values) >= 0.2
    assert np.max(values) <= 1.6


def test_sampledSet_distance_calculation(change_test_dir: Any) -> None:
    """Test that distance calculation is correct for sampledSet."""
    from pybFoam.sampling import UniformSetConfig

    _, mesh = create_time_mesh()
    search = meshSearch(mesh)

    # Create uniform line with known length using Pydantic config
    # Length = sqrt((0.5-0.1)^2 + (0.5-0.1)^2) = sqrt(0.32) ≈ 0.566
    config = UniformSetConfig(
        axis="distance", start=[0.1, 0.1, 0.005], end=[0.5, 0.5, 0.005], nPoints=11
    )
    set_dict = config.to_foam_dict()

    line = sampledSet.New(Word("testLine"), mesh, search, set_dict)

    distance = line.distance()
    distance_array = np.asarray(distance)

    # Check distance array
    assert len(distance_array) > 0
    assert distance_array[0] == pytest.approx(0.0, abs=1e-6)

    # Check that distances are monotonically increasing
    for i in range(len(distance_array) - 1):
        assert distance_array[i + 1] >= distance_array[i]


def test_coordSet_interface(change_test_dir: Any) -> None:
    """Test that coordSet interface works correctly."""
    from pybFoam.sampling import UniformSetConfig

    _, mesh = create_time_mesh()
    search = meshSearch(mesh)

    # Create uniform line using Pydantic config
    config = UniformSetConfig(
        axis="xyz", start=[0.1, 0.1, 0.005], end=[0.55, 0.55, 0.005], nPoints=15
    )
    set_dict = config.to_foam_dict()

    line = sampledSet.New(Word("coordSetTest"), mesh, search, set_dict)

    # Test coordSet methods (inherited by sampledSet)
    assert line.name() == "coordSetTest"
    assert line.axis() == "xyz"
    assert line.nPoints() > 0
    assert line.nPoints() <= 15

    points = line.points()
    assert len(points) == line.nPoints()

    distance = line.distance()
    assert len(distance) == line.nPoints()


def test_edge_cases_outside_mesh(change_test_dir: Any) -> None:
    """Test sampledSet behavior with points outside mesh."""
    from pybFoam.sampling import UniformSetConfig

    _, mesh = create_time_mesh()
    search = meshSearch(mesh)

    # Create a line that goes outside the domain using Pydantic config
    config = UniformSetConfig(
        axis="distance",
        start=[0.5, 0.5, 0.005],
        end=[1.5, 1.5, 0.005],  # Goes outside [0,1]^3 domain
        nPoints=20,
    )
    set_dict = config.to_foam_dict()

    line = sampledSet.New(Word("outsideLine"), mesh, search, set_dict)

    # sampledSet should still be created
    assert line is not None
    # Will have fewer points since many are outside
    assert line.nPoints() > 0
    assert line.nPoints() < 20

    # Some cells will be invalid (-1)
    cells = line.cells()

    # Should have some invalid cells for points outside mesh
    # (Note: sampledSet removes points completely if they're outside)
    assert len(cells) == line.nPoints()


def test_sample_multiple_fields_on_same_set(change_test_dir: Any) -> None:
    """Test sampling multiple fields on the same sampledSet."""
    time, mesh = create_time_mesh()  # time must stay alive for mesh lifetime
    search = meshSearch(mesh)

    # Create two different fields
    p_rgh = volScalarField.read_field(mesh, "p_rgh")
    U = volVectorField.read_field(mesh, "U")

    cell_centers = mesh.C()["internalField"]
    p_internal = p_rgh["internalField"]
    U_internal = U["internalField"]

    for i in range(mesh.nCells()):
        cc = cell_centers[i]
        p_internal[i] = cc[0]
        U_internal[i] = vector(cc[1], cc[2], 0.0)

    # Create single sampledSet using Pydantic config
    from pybFoam.sampling import UniformSetConfig

    config = UniformSetConfig(
        axis="distance", start=[0.15, 0.15, 0.005], end=[0.5, 0.5, 0.005], nPoints=15
    )
    set_dict = config.to_foam_dict()

    line = sampledSet.New(Word("multiFieldLine"), mesh, search, set_dict)

    # Sample both fields
    interp_p = interpolationScalar.New(Word("cellPoint"), p_rgh)
    sampled_p = sampleSetScalar(line, interp_p)

    interp_U = interpolationVector.New(Word("cellPoint"), U)
    sampled_U = sampleSetVector(line, interp_U)

    # Both should have same length
    assert len(sampled_p) == line.nPoints()
    assert len(sampled_U) == line.nPoints()

    # Both should have valid data
    p_array = np.asarray(sampled_p)
    U_array = np.asarray(sampled_U)

    valid_p = p_array < 1e10
    valid_U = np.all(U_array < 1e10, axis=1)

    # Should have same validity
    assert np.array_equal(valid_p, valid_U)
