"""pybFoam.sampling package exports for convenience.

Keep the public API small: import configs in the package namespace so
`from pybFoam.sampling import SampledPlaneConfig, UniformSetConfig` works in examples/tests.
"""

from __future__ import annotations

import sys

from pybFoam.sampling_bindings import *  # noqa: F403

from .set_configs import (
    ArraySetConfig,
    CellCentreSetConfig,
    CircleSetConfig,
    CloudSetConfig,
    FaceOnlySetConfig,
    MidPointAndFaceSetConfig,
    MidPointSetConfig,
    PatchCloudSetConfig,
    PatchSeedSetConfig,
    PolyLineSetConfig,
    SampledSetBaseConfig,
    UniformSetConfig,
    sampled_set_from,
)
from .surface_configs import (
    SampledCuttingPlaneConfig,
    SampledCuttingSurfaceConfig,
    SampledDistanceSurfaceConfig,
    SampledFaceZoneConfig,
    SampledIsoSurfaceConfig,
    SampledMeshedSurfaceConfig,
    SampledPatchConfig,
    SampledPatchInternalFieldConfig,
    SampledPlaneConfig,
    SampledSurfaceBaseConfig,
    SampledThresholdCellFacesConfig,
    sampled_surface_from,
)
from .utils import dict_to_foam

#: Sentinel value returned by ``sampleSetScalar`` / ``sampleSetVector``
#: for sample points that did not land in any cell of the mesh. This
#: mirrors OpenFOAM's ``pTraits<scalar>::max`` (i.e.
#: ``std::numeric_limits<double>::max()``). Any value at least an
#: order of magnitude smaller than this is a real interpolated result.
OUT_OF_MESH: float = sys.float_info.max

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
    # Sentinels
    "OUT_OF_MESH",
]
