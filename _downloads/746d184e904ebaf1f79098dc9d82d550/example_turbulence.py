"""
Build an incompressible turbulence model
========================================

The :mod:`pybFoam.turbulence` submodule wraps OpenFOAM's incompressible
RAS / LES factory. This how-to reads the shipped ``pitzDaily`` case
(backward-facing step, k-epsilon RAS), constructs the turbulence
model from Python, and inspects its bound state — ``nut``, ``k``,
``epsilon``, ``correct()``, ``divDevReff()``.

Prerequisite: finished
:doc:`/auto_tutorials/example_01_dictionaries`.
"""

# %%
# Clone the case
# --------------

from pybFoam import clone_example

case = clone_example("pitzDaily")

# %%
# Build mesh and read U
# ---------------------

from pybFoam import (
    Time,
    argList,
    createPhi,
    dictionary,
    fvMesh,
    volVectorField,
)
from pybFoam.meshing import generate_blockmesh

time = Time(argList([str(case), "-case", str(case)]))
generate_blockmesh(time, dictionary.read(str(case / "system" / "blockMeshDict")))
mesh = fvMesh(time)

U = volVectorField.read_field(mesh, "U")
phi = createPhi(U)

# %%
# Construct the transport and turbulence models
# ---------------------------------------------
# ``singlePhaseTransportModel(U, phi)`` reads the Newtonian ``nu`` from
# ``constant/transportProperties``. ``incompressibleTurbulenceModel.New``
# dispatches on ``simulationType`` and ``RASModel`` from
# ``turbulenceProperties`` to instantiate kEpsilon, kOmegaSST, etc.

from pybFoam.turbulence import (
    incompressibleTurbulenceModel,
    singlePhaseTransportModel,
)

transport = singlePhaseTransportModel(U, phi)
model = incompressibleTurbulenceModel.New(U, phi, transport)

# %%
# Inspect the model
# -----------------
# Each accessor returns a ``tmp<volScalarField>``; call ``()`` to get
# the underlying field, then ``["internalField"]`` for the NumPy view.

import numpy as np

nut = np.asarray(model.nut()()["internalField"])
k = np.asarray(model.k()()["internalField"])
eps = np.asarray(model.epsilon()()["internalField"])

print(f"nut       : shape={nut.shape}   min={nut.min():.3e}   max={nut.max():.3e}")
print(f"k         : shape={k.shape}   min={k.min():.3e}   max={k.max():.3e}")
print(f"epsilon   : shape={eps.shape}   min={eps.min():.3e}   max={eps.max():.3e}")

# %%
# Update the eddy viscosity
# -------------------------
# ``correct()`` is what a PISO/PIMPLE solver calls at the end of every
# outer iteration. For k-epsilon it solves the k and epsilon transport
# equations and recomputes ``nut = Cmu * k^2 / epsilon``.

model.correct()

nut_after = np.asarray(model.nut()()["internalField"])
print(f"nut after correct() : min={nut_after.min():.3e}   max={nut_after.max():.3e}")

# %%
# divDevReff in a momentum equation
# ---------------------------------
# In a turbulent solver the laminar diffusion ``laplacian(nu, U)`` is
# replaced by the model's stress divergence:
#
# .. code-block:: python
#
#    UEqn = fvVectorMatrix(fvm.ddt(U) + fvm.div(phi, U) + model.divDevReff(U))

UEqn_term = model.divDevReff(U)
print(f"divDevReff term : {type(UEqn_term).__name__}")

# %%
# See also
# --------
#
# - :doc:`example_modify_boundary_conditions` — change a patch field
#   (e.g. inlet velocity) before constructing the turbulence model.
# - :doc:`/auto_tutorials/example_04_run_and_sample` — drive a solver
#   loop; add ``model.correct()`` inside it for a turbulent run.
