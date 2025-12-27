import pytest

from pybFoam.sampling import (
    UniformSetConfig,
    CloudSetConfig,
    PolyLineSetConfig,
    CircleSetConfig,
    ArraySetConfig,
    FaceOnlySetConfig,
    MidPointSetConfig,
    CellCentreSetConfig,
    PatchCloudSetConfig,
    PatchSeedSetConfig,
    sampled_set_from,
)


def test_uniform_set_valid():
    """Test valid uniform set configuration."""
    config = UniformSetConfig(
        axis="distance",
        start=[0.0, 0.0, 0.0],
        end=[1.0, 1.0, 0.0],
        nPoints=20
    )
    d = config.model_dump(exclude_none=True)
    assert d["type"] == "uniform"
    assert d["axis"] == "distance"
    assert d["start"] == [0.0, 0.0, 0.0]
    assert d["end"] == [1.0, 1.0, 0.0]
    assert d["nPoints"] == 20


def test_uniform_set_with_tolerance():
    """Test uniform set with optional tolerance."""
    config = UniformSetConfig(
        axis="xyz",
        start=[0.0, 0.0, 0.0],
        end=[1.0, 0.0, 0.0],
        nPoints=10,
        tol=1e-4
    )
    d = config.model_dump(exclude_none=True)
    assert d["tol"] == 1e-4


def test_uniform_set_invalid_point_length():
    """Test uniform set with invalid point length."""
    with pytest.raises(ValueError, match="must have length 3"):
        UniformSetConfig(
            axis="x",
            start=[0.0, 0.0],  # Only 2 elements
            end=[1.0, 0.0, 0.0],
            nPoints=10
        )


def test_cloud_set_valid():
    """Test valid cloud set configuration."""
    config = CloudSetConfig(
        axis="xyz",
        points=[
            [0.1, 0.1, 0.0],
            [0.5, 0.5, 0.0],
            [0.9, 0.9, 0.0]
        ]
    )
    d = config.model_dump(exclude_none=True)
    assert d["type"] == "cloud"
    assert len(d["points"]) == 3


def test_cloud_set_empty_points():
    """Test cloud set with empty points list."""
    with pytest.raises(ValueError, match="cannot be empty"):
        CloudSetConfig(axis="xyz", points=[])


def test_polyline_set_valid():
    """Test valid polyline set configuration."""
    config = PolyLineSetConfig(
        axis="distance",
        points=[
            [0.0, 0.0, 0.0],
            [1.0, 0.0, 0.0],
            [1.0, 1.0, 0.0]
        ],
        nPoints=50
    )
    d = config.model_dump(exclude_none=True)
    assert d["type"] == "polyLine"
    assert len(d["points"]) == 3
    assert d["nPoints"] == 50


def test_polyline_set_insufficient_points():
    """Test polyline set with insufficient points."""
    with pytest.raises(ValueError, match="at least 2 points"):
        PolyLineSetConfig(
            axis="distance",
            points=[[0.0, 0.0, 0.0]]  # Only 1 point
        )


def test_circle_set_valid():
    """Test valid circle set configuration."""
    config = CircleSetConfig(
        axis="distance",
        origin=[0.5, 0.5, 0.0],
        circleAxis=[0.0, 0.0, 1.0],
        startPoint=[0.6, 0.5, 0.0],
        dTheta=10.0
    )
    d = config.model_dump(exclude_none=True)
    assert d["type"] == "circle"
    assert d["origin"] == [0.5, 0.5, 0.0]
    assert d["circleAxis"] == [0.0, 0.0, 1.0]
    assert d["dTheta"] == 10.0


def test_array_set_valid():
    """Test valid array set configuration."""
    config = ArraySetConfig(
        axis="xyz",
        pointsDensity=[10, 10, 5],
        spanBox=[1.0, 1.0, 0.5],
        origin=[0.0, 0.0, 0.0]
    )
    d = config.model_dump(exclude_none=True)
    assert d["type"] == "array"
    assert d["pointsDensity"] == [10, 10, 5]
    assert d["spanBox"] == [1.0, 1.0, 0.5]


def test_array_set_invalid_density_length():
    """Test array set with invalid density length."""
    with pytest.raises(ValueError, match="must have 3 integer values"):
        ArraySetConfig(
            axis="xyz",
            pointsDensity=[10, 10],  # Only 2 values
            spanBox=[1.0, 1.0, 0.5]
        )


def test_face_only_set_valid():
    """Test valid faceOnly set configuration."""
    config = FaceOnlySetConfig(
        axis="distance",
        start=[0.0, 0.0, 0.0],
        end=[1.0, 1.0, 0.0]
    )
    d = config.model_dump(exclude_none=True)
    assert d["type"] == "face"
    assert d["start"] == [0.0, 0.0, 0.0]
    assert d["end"] == [1.0, 1.0, 0.0]


def test_midpoint_set_valid():
    """Test valid midPoint set configuration."""
    config = MidPointSetConfig(
        axis="distance",
        start=[0.0, 0.0, 0.0],
        end=[1.0, 1.0, 0.0]
    )
    d = config.model_dump(exclude_none=True)
    assert d["type"] == "midPoint"


def test_cell_centre_set_valid():
    """Test valid cellCentre set configuration."""
    config = CellCentreSetConfig(
        axis="distance",
        start=[0.1, 0.1, 0.0],
        end=[0.9, 0.9, 0.0]
    )
    d = config.model_dump(exclude_none=True)
    assert d["type"] == "cellCentre"


def test_patch_cloud_set_valid():
    """Test valid patchCloud set configuration."""
    config = PatchCloudSetConfig(
        axis="xyz",
        patches=["inlet", "outlet"],
        points=[
            [0.0, 0.0, 0.0],
            [0.5, 0.5, 0.0]
        ],
        maxDistance=0.1
    )
    d = config.model_dump(exclude_none=True)
    assert d["type"] == "patchCloud"
    assert d["patches"] == ["inlet", "outlet"]
    assert d["maxDistance"] == 0.1


def test_patch_seed_set_valid():
    """Test valid patchSeed set configuration."""
    config = PatchSeedSetConfig(
        axis="distance",
        patches="inlet",
        searchDir=[1.0, 0.0, 0.0],
        maxDistance=1.0
    )
    d = config.model_dump(exclude_none=True)
    assert d["type"] == "patchSeed"
    assert d["patches"] == "inlet"
    assert d["searchDir"] == [1.0, 0.0, 0.0]


def test_sampled_set_from_helper():
    """Test sampled_set_from helper function."""
    config = UniformSetConfig(
        axis="x",
        start=[0.0, 0.0, 0.0],
        end=[1.0, 0.0, 0.0],
        nPoints=10
    )
    d = sampled_set_from(config)
    assert d["type"] == "uniform"
    assert d["axis"] == "x"
    
    # Test with plain dict
    plain_dict = {"type": "uniform", "axis": "y"}
    result = sampled_set_from(plain_dict)
    assert result is plain_dict


@pytest.mark.parametrize("axis_type", ["x", "y", "z", "xyz", "distance"])
def test_uniform_set_axis_types(axis_type):
    """Test uniform set with all valid axis types."""
    config = UniformSetConfig(
        axis=axis_type,
        start=[0.0, 0.0, 0.0],
        end=[1.0, 1.0, 1.0],
        nPoints=20
    )
    d = config.model_dump(exclude_none=True)
    assert d["axis"] == axis_type
