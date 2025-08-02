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
__all__ = ['__version__', 'fvc', 'runTimeTables', 'thermo', 'turbulence', 
           'postProcess', 'time_series', 'fieldFunctions']

try:
    from . import fvc
except ImportError as e:
    raise ImportError(f"Could not import fvc module. Make sure OpenFOAM environment is sourced: {e}")

try:
    from . import runTimeTables
except ImportError as e:
    raise ImportError(f"Could not import runTimeTables module. Make sure OpenFOAM environment is sourced: {e}")

try:
    from . import thermo
except ImportError as e:
    raise ImportError(f"Could not import thermo module. Make sure OpenFOAM environment is sourced: {e}")

try:
    from . import turbulence
except ImportError as e:
    raise ImportError(f"Could not import turbulence module. Make sure OpenFOAM environment is sourced: {e}")    

try:
    from . import postProcess
except ImportError as e:
    raise ImportError(f"Could not import postProcess module: {e}")

try:
    from . import time_series
except ImportError as e:    
    raise ImportError(f"Could not import time_series module: {e}")

try: 
    from . import fieldFunctions
except ImportError as e:
    raise ImportError(f"Could not import fieldFunctions module: {e}")
