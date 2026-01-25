from ._version import __version__
import sys
import os


from pybFoam.pybFoam_core import *

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
from . import sampling_bindings
