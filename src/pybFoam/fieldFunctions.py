from pybFoam import volScalarField, fvMesh, volSymmTensorField
# from pybFoam.thermo import fluidThermo
from pybFoam.turbulence import incompressibleTurbulenceModel, compressibleTurbulenceModel
# from pydantic import BaseModel
# import pybFoam
# import numpy as np
# from typing import Protocol, List, Any, Callable
# from pathlib import Path
# import os

def pressure(mesh: fvMesh, p_name: str = "p"):
    p = volScalarField.from_registry(mesh,p_name)
    return p

def viscousStressTensorEff(mesh: fvMesh):
    turb = compressibleTurbulenceModel.from_registry(mesh)
    return volSymmTensorField(turb.devRhoReff())



# def viscousForce(mesh: fvMesh, p_name: str = "p"):
#     p = volScalarField.from_registry(mesh,p_name)
#     return mesh.Sf() * p