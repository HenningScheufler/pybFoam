from __future__ import annotations

from typing import Any, Dict, List, Literal, Optional, Union

from pydantic import BaseModel, ConfigDict, Field, field_validator

from pybFoam import dictionary, vector, vectorField

from .utils import _ensure_len, dict_to_foam


class SampledSetBaseConfig(BaseModel):
    """Base configuration for all sampledSet types."""

    model_config = ConfigDict(extra="allow")

    type: str = Field(..., description="sampledSet type")
    axis: Literal["x", "y", "z", "xyz", "distance"] = Field(
        ..., description="Output type of sample locations"
    )
    name: Optional[str] = Field(None, description="optional label/name")


class UniformSetConfig(SampledSetBaseConfig):
    """Configuration for uniform (straight line) sampledSet."""

    type: Literal["uniform"] = Field("uniform", description="uniform sampledSet type")
    start: List[float] = Field(..., description="Start point of sample line")
    end: List[float] = Field(..., description="End point of sample line")
    nPoints: int = Field(..., description="Number of points between start/end", gt=0)
    tol: Optional[float] = Field(None, description="Relative tolerance", gt=0)

    @field_validator("start", "end")
    @classmethod
    def point_len(cls, v: List[float]) -> List[float]:
        return _ensure_len(v, 3, "start/end point")

    def to_foam_dict(self) -> Any:
        """Return OpenFOAM dictionary object."""
        return dict_to_foam(self.model_dump(exclude_none=True))


class CloudSetConfig(SampledSetBaseConfig):
    """Configuration for cloud (arbitrary points) sampledSet."""

    type: Literal["cloud"] = Field("cloud", description="cloud sampledSet type")
    points: List[List[float]] = Field(..., description="List of sampling points")

    @field_validator("points")
    @classmethod
    def validate_points(cls, v: List[List[float]]) -> List[List[float]]:
        if not v:
            raise ValueError("points list cannot be empty")
        return [_ensure_len(pt, 3, f"point {i}") for i, pt in enumerate(v)]

    def to_foam_dict(self) -> dictionary:
        """Return OpenFOAM dictionary object."""

        d = self.model_dump(exclude_none=True)
        # Convert points list to vectorField
        points_list = d.pop("points")
        foam_dict = dict_to_foam(d)

        # Create vectorField from points
        vf = vectorField([vector(p[0], p[1], p[2]) for p in points_list])
        foam_dict.set("points", vf)

        return foam_dict


class PolyLineSetConfig(SampledSetBaseConfig):
    """Configuration for polyLine (multi-segment) sampledSet."""

    type: Literal["polyLine"] = Field("polyLine", description="polyLine sampledSet type")
    points: List[List[float]] = Field(..., description="List of knot points defining the polyline")
    nPoints: Optional[int] = Field(None, description="Total number of sample points", gt=0)

    @field_validator("points")
    @classmethod
    def validate_points(cls, v: List[List[float]]) -> List[List[float]]:
        if len(v) < 2:
            raise ValueError("polyLine requires at least 2 points")
        return [_ensure_len(pt, 3, f"point {i}") for i, pt in enumerate(v)]

    def to_foam_dict(self) -> dictionary:
        """Return OpenFOAM dictionary object."""

        d = self.model_dump(exclude_none=True)
        points_list = d.pop("points")
        foam_dict = dict_to_foam(d)

        # Create vectorField from points
        vf = vectorField([vector(p[0], p[1], p[2]) for p in points_list])
        foam_dict.set("points", vf)

        return foam_dict


class CircleSetConfig(SampledSetBaseConfig):
    """Configuration for circle sampledSet."""

    type: Literal["circle"] = Field("circle", description="circle sampledSet type")
    origin: List[float] = Field(..., description="Origin (center) of the circle")
    circleAxis: List[float] = Field(..., description="Axis (normal) of the circle")
    startPoint: List[float] = Field(..., description="Starting point on the circle")
    dTheta: float = Field(..., description="Sampling increment in degrees", gt=0)

    @field_validator("origin", "circleAxis", "startPoint")
    @classmethod
    def point_len(cls, v: List[float]) -> List[float]:
        return _ensure_len(v, 3, "vector")

    def to_foam_dict(self) -> Any:
        """Return OpenFOAM dictionary object."""
        return dict_to_foam(self.model_dump(exclude_none=True))


class ArraySetConfig(SampledSetBaseConfig):
    """Configuration for array (3D grid) sampledSet."""

    type: Literal["array"] = Field("array", description="array sampledSet type")
    pointsDensity: List[int] = Field(..., description="Sampling density as (x y z) integers")
    spanBox: List[float] = Field(..., description="Sample box dimensions")
    origin: Optional[List[float]] = Field(None, description="Origin of coordinate system")

    @field_validator("pointsDensity")
    @classmethod
    def density_len(cls, v: List[int]) -> List[int]:
        if len(v) != 3:
            raise ValueError("pointsDensity must have 3 integer values")
        return [int(x) for x in v]

    @field_validator("spanBox")
    @classmethod
    def span_len(cls, v: List[float]) -> List[float]:
        return _ensure_len(v, 3, "spanBox")

    @field_validator("origin")
    @classmethod
    def origin_len(cls, v: Optional[List[float]]) -> Optional[List[float]]:
        if v is None:
            return v
        return _ensure_len(v, 3, "origin")

    def to_foam_dict(self) -> Any:
        """Return OpenFOAM dictionary object."""
        return dict_to_foam(self.model_dump(exclude_none=True))


class FaceOnlySetConfig(SampledSetBaseConfig):
    """Configuration for faceOnly sampledSet."""

    type: Literal["face", "faceOnly"] = Field("face", description="faceOnly sampledSet type")
    start: List[float] = Field(..., description="Start point")
    end: List[float] = Field(..., description="End point")

    @field_validator("start", "end")
    @classmethod
    def point_len(cls, v: List[float]) -> List[float]:
        return _ensure_len(v, 3, "point")

    def to_foam_dict(self) -> Any:
        """Return OpenFOAM dictionary object."""
        return dict_to_foam(self.model_dump(exclude_none=True))


class MidPointSetConfig(SampledSetBaseConfig):
    """Configuration for midPoint sampledSet."""

    type: Literal["midPoint"] = Field("midPoint", description="midPoint sampledSet type")
    start: List[float] = Field(..., description="Start point")
    end: List[float] = Field(..., description="End point")

    @field_validator("start", "end")
    @classmethod
    def point_len(cls, v: List[float]) -> List[float]:
        return _ensure_len(v, 3, "point")

    def to_foam_dict(self) -> Any:
        """Return OpenFOAM dictionary object."""
        return dict_to_foam(self.model_dump(exclude_none=True))


class MidPointAndFaceSetConfig(SampledSetBaseConfig):
    """Configuration for midPointAndFace sampledSet."""

    type: Literal["midPointAndFace"] = Field(
        "midPointAndFace", description="midPointAndFace sampledSet type"
    )
    start: List[float] = Field(..., description="Start point")
    end: List[float] = Field(..., description="End point")

    @field_validator("start", "end")
    @classmethod
    def point_len(cls, v: List[float]) -> List[float]:
        return _ensure_len(v, 3, "point")

    def to_foam_dict(self) -> Any:
        """Return OpenFOAM dictionary object."""
        return dict_to_foam(self.model_dump(exclude_none=True))


class CellCentreSetConfig(SampledSetBaseConfig):
    """Configuration for cellCentre sampledSet."""

    type: Literal["cellCentre"] = Field("cellCentre", description="cellCentre sampledSet type")
    start: List[float] = Field(..., description="Start point")
    end: List[float] = Field(..., description="End point")

    @field_validator("start", "end")
    @classmethod
    def point_len(cls, v: List[float]) -> List[float]:
        return _ensure_len(v, 3, "point")

    def to_foam_dict(self) -> Any:
        """Return OpenFOAM dictionary object."""
        return dict_to_foam(self.model_dump(exclude_none=True))


class PatchCloudSetConfig(SampledSetBaseConfig):
    """Configuration for patchCloud sampledSet."""

    type: Literal["patchCloud"] = Field("patchCloud", description="patchCloud sampledSet type")
    patches: Union[str, List[str]] = Field(..., description="Patch selection")
    points: List[List[float]] = Field(..., description="Sampling points")
    maxDistance: Optional[float] = Field(None, description="Maximum distance to patch", gt=0)

    @field_validator("patches")
    @classmethod
    def patches_list(cls, v: Union[str, List[str]]) -> Union[str, List[str]]:
        if isinstance(v, str):
            return v
        return [str(x) for x in v]

    @field_validator("points")
    @classmethod
    def validate_points(cls, v: List[List[float]]) -> List[List[float]]:
        if not v:
            raise ValueError("points list cannot be empty")
        return [_ensure_len(pt, 3, f"point {i}") for i, pt in enumerate(v)]

    def to_foam_dict(self) -> Any:
        """Return OpenFOAM dictionary object."""

        d = self.model_dump(exclude_none=True)
        points_list = d.pop("points")
        foam_dict = dict_to_foam(d)

        vf = vectorField([vector(p[0], p[1], p[2]) for p in points_list])
        foam_dict.set("points", vf)

        return foam_dict


class PatchSeedSetConfig(SampledSetBaseConfig):
    """Configuration for patchSeed sampledSet."""

    type: Literal["patchSeed"] = Field("patchSeed", description="patchSeed sampledSet type")
    patches: Union[str, List[str]] = Field(..., description="Patch selection")
    searchDir: List[float] = Field(..., description="Search direction")
    maxDistance: Optional[float] = Field(None, description="Maximum distance", gt=0)

    @field_validator("patches")
    @classmethod
    def patches_list(cls, v: Union[str, List[str]]) -> Union[str, List[str]]:
        if isinstance(v, str):
            return v
        return [str(x) for x in v]

    @field_validator("searchDir")
    @classmethod
    def dir_len(cls, v: List[float]) -> List[float]:
        return _ensure_len(v, 3, "searchDir")

    def to_foam_dict(self) -> Any:
        """Return OpenFOAM dictionary object."""
        return dict_to_foam(self.model_dump(exclude_none=True))


def sampled_set_from(obj: Union[Dict[str, Any], SampledSetBaseConfig]) -> Dict[str, Any]:
    """Normalize input to a plain dict for bindings.

    Accepts either a plain dict (returned unchanged) or a Pydantic model (converted).
    """
    if isinstance(obj, dict):
        return obj
    return obj.model_dump(exclude_none=True)


__all__ = [
    "SampledSetBaseConfig",
    "UniformSetConfig",
    "CloudSetConfig",
    "PolyLineSetConfig",
    "CircleSetConfig",
    "ArraySetConfig",
    "FaceOnlySetConfig",
    "MidPointSetConfig",
    "MidPointAndFaceSetConfig",
    "CellCentreSetConfig",
    "PatchCloudSetConfig",
    "PatchSeedSetConfig",
    "sampled_set_from",
]
