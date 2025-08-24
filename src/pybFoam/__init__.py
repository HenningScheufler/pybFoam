from ._version import __version__
import sys
import os

# Core modules that should always be available
try:
    from pybFoam.pybFoam_core import *
except ImportError as e:
    print(f"Warning: Could not import pybFoam_core: {e}")

# Optional modules that require OpenFOAM environment
# These will be imported on-demand
__all__ = [
    "__version__",
    "fvc",
    "fvm",
    "runTimeTables",
    "thermo",
    "turbulence"
]

from . import fvc
from . import fvm
from . import runTimeTables
from . import thermo
from . import turbulence
