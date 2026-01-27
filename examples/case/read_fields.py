#%%
from pybFoam import (
    Time, fvMesh, volScalarField, volVectorField,
    dictionary, Word, vector, wordList
)
import numpy as np


def create_time_mesh():
    """Create OpenFOAM mesh from test case."""
    time = Time(".", ".")
    return time, fvMesh(time)


time, mesh = create_time_mesh()
#%%
alpha = volScalarField.read_field(mesh, "alpha.water")
np_alpha = np.asarray(alpha["internalField"])
np_alpha
# %%
