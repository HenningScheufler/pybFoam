import pytest

from pybFoam.sampling import (
    SampledCuttingPlaneConfig,
    SampledDistanceSurfaceConfig,
    SampledFaceZoneConfig,
    SampledIsoSurfaceConfig,
    SampledPatchConfig,
    SampledPlaneConfig,
    SampledThresholdCellFacesConfig,
    sampled_surface_from,
)


def test_sampled_plane_valid() -> None:
    p = SampledPlaneConfig(origin=[0, 0, 0], normal=[0, 0, 1])
    d = p.model_dump(exclude_none=True)
    assert d["type"] == "plane"
    assert d["origin"] == [0.0, 0.0, 0.0]
    assert d["normal"] == [0.0, 0.0, 1.0]


def test_sampled_plane_invalid_origin() -> None:
    with pytest.raises(ValueError):
        SampledPlaneConfig(origin=[0, 0], normal=[0, 0, 1])


def test_sampled_patch_and_helper() -> None:
    patch = SampledPatchConfig(patches=["inlet", "outlet"])
    d = sampled_surface_from(patch)
    assert d["type"] == "patch"
    assert "patches" in d


def test_sampled_cutting_plane() -> None:
    cp = SampledCuttingPlaneConfig(point=[0, 0, 0], normal=[1, 0, 0], isoMethod="topo")
    d = cp.model_dump(exclude_none=True)
    assert d["type"] == "cuttingPlane"
    assert d["point"] == [0.0, 0.0, 0.0]
    assert d["normal"] == [1.0, 0.0, 0.0]
    assert d["isoMethod"] == "topo"


def test_sampled_iso_surface() -> None:
    iso = SampledIsoSurfaceConfig(isoField="T", isoValue=373.0)
    d = iso.model_dump(exclude_none=True)
    assert d["type"] == "isoSurface"
    assert d["isoField"] == "T"
    assert d["isoValue"] == 373.0


def test_sampled_face_zone() -> None:
    fz = SampledFaceZoneConfig(zones=["zone1", "zone2"])
    d = fz.model_dump(exclude_none=True)
    assert d["type"] == "faceZone"
    assert d["zones"] == ["zone1", "zone2"]


def test_sampled_threshold_cell_faces() -> None:
    tcf = SampledThresholdCellFacesConfig(field="rho", lowerLimit=0.1, upperLimit=10.0)
    d = tcf.model_dump(exclude_none=True)
    assert d["type"] == "thresholdCellFaces"
    assert d["field"] == "rho"
    assert d["lowerLimit"] == 0.1
    assert d["upperLimit"] == 10.0


def test_sampled_distance_surface() -> None:
    ds = SampledDistanceSurfaceConfig(
        surfaceType="triSurfaceMesh", surfaceName="sphere.obj", distance=0.01
    )
    d = ds.model_dump(exclude_none=True)
    assert d["type"] == "distanceSurface"
    assert d["surfaceType"] == "triSurfaceMesh"
    assert d["distance"] == 0.01
