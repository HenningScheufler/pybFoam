"""pybFoam.sampling package exports for convenience.

Keep the public API small: import configs in the package namespace so
`from pybFoam.sampling import SampledPlaneConfig, UniformSetConfig` works in examples/tests.
"""
from __future__ import annotations


from pybFoam.sampling_bindings import *  # noqa: F403

from .surface_configs import (
    SampledSurfaceBaseConfig,
    SampledPlaneConfig,
    SampledPatchConfig,
    SampledCuttingPlaneConfig,
    SampledCuttingSurfaceConfig,
    SampledIsoSurfaceConfig,
    SampledMeshedSurfaceConfig,
    SampledFaceZoneConfig,
    SampledPatchInternalFieldConfig,
    SampledThresholdCellFacesConfig,
    SampledDistanceSurfaceConfig,
    sampled_surface_from,
)

from .set_configs import (
    SampledSetBaseConfig,
    UniformSetConfig,
    CloudSetConfig,
    PolyLineSetConfig,
    CircleSetConfig,
    ArraySetConfig,
    FaceOnlySetConfig,
    MidPointSetConfig,
    MidPointAndFaceSetConfig,
    CellCentreSetConfig,
    PatchCloudSetConfig,
    PatchSeedSetConfig,
    sampled_set_from,
)

from .utils import dict_to_foam

__all__ = [
    # SampledSurface configs
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
    # SampledSet configs
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
    # Helper functions
    "sampled_surface_from",
    "sampled_set_from",
    "dict_to_foam",
]
