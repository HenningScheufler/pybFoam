from __future__ import annotations

from typing import Any, Dict, List, Literal, Optional, Union

from pydantic import BaseModel, ConfigDict, Field, field_validator

from pybFoam import dictionary

from .utils import _ensure_len, dict_to_foam


class SampledSurfaceBaseConfig(BaseModel):
    model_config = ConfigDict(extra="allow")

    type: str = Field(..., description="sampledSurface type, e.g. 'plane' or 'patch'")
    name: Optional[str] = Field(None, description="optional label/name")


class SampledPlaneConfig(SampledSurfaceBaseConfig):
    type: Literal["plane"] = Field("plane", description="plane sampledSurface type")
    planeType: Optional[str] = Field(None, description="plane description (pointAndNormal, etc)")
    point: Optional[List[float]] = Field(None, description="point on plane (alternative to origin)")
    origin: Optional[List[float]] = Field(None, description="3-element origin vector")
    normal: Optional[List[float]] = Field(None, description="3-element normal vector")
    triangulate: Optional[bool] = Field(None, description="triangulate faces")
    bounds: Optional[List[List[float]]] = Field(None, description="bounding box")
    zone: Optional[str] = Field(None, description="limit to cell zone")
    zones: Optional[List[str]] = Field(None, description="limit to cell zones")

    @field_validator("point", "origin")
    @classmethod
    def point_len(cls, v: Optional[List[float]]) -> Optional[List[float]]:
        if v is None:
            return v
        return _ensure_len(v, 3, "point/origin")

    @field_validator("normal")
    @classmethod
    def normal_len(cls, v: Optional[List[float]]) -> Optional[List[float]]:
        if v is None:
            return v
        return _ensure_len(v, 3, "normal")

    def to_foam_dict(self) -> dictionary:
        """Return OpenFOAM dictionary object."""
        return dict_to_foam(self.model_dump(exclude_none=True))


class SampledPatchConfig(SampledSurfaceBaseConfig):
    type: Literal["patch"] = Field("patch", description="patch sampledSurface type")
    patches: Union[str, List[str]] = Field(..., description="patch selection as word/regex list")
    triangulate: Optional[bool] = Field(None, description="triangulate faces")

    @field_validator("patches")
    @classmethod
    def patches_list(cls, v: Union[str, List[str]]) -> Union[str, List[str]]:
        if isinstance(v, str):
            return v
        return [str(x) for x in v]

    def to_foam_dict(self) -> dictionary:
        """Return OpenFOAM dictionary object."""
        return dict_to_foam(self.model_dump(exclude_none=True))


class SampledCuttingPlaneConfig(SampledSurfaceBaseConfig):
    type: Literal["cuttingPlane"] = Field(
        "cuttingPlane", description="cuttingPlane sampledSurface type"
    )
    planeType: Optional[str] = Field(None, description="plane description (pointAndNormal, etc)")
    point: Optional[List[float]] = Field(None, description="point on plane")
    origin: Optional[List[float]] = Field(None, description="3-element origin vector")
    normal: Optional[List[float]] = Field(None, description="3-element normal vector")
    offsets: Optional[List[float]] = Field(
        None, description="offsets of the origin in normal direction"
    )
    isoMethod: Optional[Literal["cell", "topo", "point"]] = Field(None, description="iso-algorithm")
    bounds: Optional[List[List[float]]] = Field(None, description="bounding box")
    zone: Optional[str] = Field(None, description="limit to cell zone")
    zones: Optional[List[str]] = Field(None, description="limit to cell zones")
    regularise: Optional[bool] = Field(None, description="face simplification")
    mergeTol: Optional[float] = Field(None, description="tolerance for merging points")

    @field_validator("point", "origin")
    @classmethod
    def point_len(cls, v: Optional[List[float]]) -> Optional[List[float]]:
        if v is None:
            return v
        return _ensure_len(v, 3, "point/origin")

    @field_validator("normal")
    @classmethod
    def normal_len(cls, v: Optional[List[float]]) -> Optional[List[float]]:
        if v is None:
            return v
        return _ensure_len(v, 3, "normal")

    def to_foam_dict(self) -> Any:
        """Return OpenFOAM dictionary object."""
        return dict_to_foam(self.model_dump(exclude_none=True))


class SampledIsoSurfaceConfig(SampledSurfaceBaseConfig):
    type: Literal["isoSurface"] = Field("isoSurface", description="isoSurface sampledSurface type")
    isoField: str = Field(..., description="field name for obtaining iso-surface")
    isoValue: Optional[float] = Field(None, description="value of iso-surface")
    isoValues: Optional[List[float]] = Field(None, description="values for iso-surfaces")
    isoMethod: Optional[Literal["cell", "topo", "point"]] = Field(None, description="iso-algorithm")
    average: Optional[bool] = Field(None, description="cell values from averaged point values")
    bounds: Optional[List[List[float]]] = Field(None, description="bounding box")
    zone: Optional[str] = Field(None, description="limit to cell zone")
    zones: Optional[List[str]] = Field(None, description="limit to cell zones")
    regularise: Optional[bool] = Field(None, description="point snapping")
    triangulate: Optional[bool] = Field(None, description="triangulate faces (if regularise)")
    mergeTol: Optional[float] = Field(None, description="tolerance for merging points")

    def to_foam_dict(self) -> Any:
        """Return OpenFOAM dictionary object."""
        return dict_to_foam(self.model_dump(exclude_none=True))


class SampledMeshedSurfaceConfig(SampledSurfaceBaseConfig):
    type: Literal["meshedSurface"] = Field(
        "meshedSurface", description="meshedSurface sampledSurface type"
    )
    surface: Optional[str] = Field(None, description="surface name in triSurface/")
    source: Optional[Literal["cells", "insideCells", "boundaryFaces"]] = Field(
        None, description="sampling source"
    )
    patches: Optional[List[str]] = Field(None, description="limit to named surface regions")
    maxDistance: Optional[float] = Field(None, description="max sampling distance")
    interpolate: Optional[bool] = Field(None, description="interpolate values")


class SampledCuttingSurfaceConfig(SampledSurfaceBaseConfig):
    type: Literal["cuttingSurface"] = Field(
        "cuttingSurface", description="cuttingSurface sampledSurface type"
    )
    surfaceType: str = Field(..., description="type of surface")
    surfaceName: Optional[str] = Field(None, description="name of surface in triSurface/")
    triangulate: Optional[bool] = Field(None, description="triangulate faces")
    bounds: Optional[List[List[float]]] = Field(
        None, description="bounding box [[xmin,ymin,zmin],[xmax,ymax,zmax]]"
    )
    zone: Optional[str] = Field(None, description="limit to cell zone")
    zones: Optional[List[str]] = Field(None, description="limit to cell zones")


class SampledFaceZoneConfig(SampledSurfaceBaseConfig):
    type: Literal["faceZone", "faceZones"] = Field(
        "faceZone", description="faceZone sampledSurface type"
    )
    zones: List[str] = Field(..., description="zone selection as word/regex list")
    triangulate: Optional[bool] = Field(None, description="triangulate faces")


class SampledPatchInternalFieldConfig(SampledSurfaceBaseConfig):
    type: Literal["patchInternalField"] = Field(
        "patchInternalField", description="patchInternalField sampledSurface type"
    )
    patches: Union[str, List[str]] = Field(..., description="patch selection")
    offsetMode: Optional[Literal["normal", "uniform", "nonuniform"]] = Field(
        None, description="offset mode"
    )
    distance: Optional[float] = Field(None, description="distance for normal offset")
    offset: Optional[List[float]] = Field(None, description="point offset for uniform offset")
    offsets: Optional[List[List[float]]] = Field(
        None, description="point offsets for nonuniform offset"
    )


class SampledThresholdCellFacesConfig(SampledSurfaceBaseConfig):
    type: Literal["thresholdCellFaces"] = Field(
        "thresholdCellFaces", description="thresholdCellFaces sampledSurface type"
    )
    field: str = Field(..., description="field name for threshold")
    lowerLimit: Optional[float] = Field(None, description="lower limit for threshold")
    upperLimit: Optional[float] = Field(None, description="upper limit for threshold")
    triangulate: Optional[bool] = Field(None, description="triangulate faces")


class SampledDistanceSurfaceConfig(SampledSurfaceBaseConfig):
    type: Literal["distanceSurface"] = Field(
        "distanceSurface", description="distanceSurface sampledSurface type"
    )
    surfaceType: str = Field(..., description="type of surface")
    surfaceName: Optional[str] = Field(None, description="name of surface in triSurface/")
    distance: Optional[float] = Field(None, description="distance from surface")
    signed: Optional[bool] = Field(None, description="use sign when distance is positive")
    isoMethod: Optional[Literal["cell", "topo", "point"]] = Field(None, description="iso-algorithm")
    regularise: Optional[bool] = Field(None, description="face simplification")
    average: Optional[bool] = Field(None, description="cell values from averaged point values")
    bounds: Optional[List[List[float]]] = Field(None, description="bounding box")
    topology: Optional[str] = Field(None, description="topology filter name")


def sampled_surface_from(obj: Union[Dict[str, Any], SampledSurfaceBaseConfig]) -> Dict[str, Any]:
    """Normalize input to a plain dict for bindings.

    Accepts either a plain dict (returned unchanged) or a Pydantic model (converted).
    """
    if isinstance(obj, dict):
        return obj
    return obj.model_dump(exclude_none=True)


__all__ = [
    "SampledSurfaceBaseConfig",
    "SampledPlaneConfig",
    "SampledPatchConfig",
    "SampledCuttingPlaneConfig",
    "SampledCuttingSurfaceConfig",
    "SampledIsoSurfaceConfig",
    "SampledMeshedSurfaceConfig",
    "SampledFaceZoneConfig",
    "SampledPatchInternalFieldConfig",
    "SampledThresholdCellFacesConfig",
    "SampledDistanceSurfaceConfig",
    "sampled_surface_from",
]
