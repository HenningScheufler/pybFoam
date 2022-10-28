from ._version import __version__
import sys
import os 

class pybFoamPaths(object):
    pybFoam_install_path = os.path.abspath(os.path.join(os.path.dirname(__file__)))

    pybFoam_libs = os.path.join(pybFoam_install_path)

sys.path.append(pybFoamPaths.pybFoam_libs)

from pybFoam_core import *
from pybFoam import postProcess
from pybFoam import time_series