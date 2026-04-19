"""
Compute finite-volume operators from Python
===========================================

:mod:`pybFoam.fvc` and :mod:`pybFoam.fvm` mirror OpenFOAM's C++ operator
API. ``fvc`` evaluates explicit operators and returns a ``tmp_*`` field
wrapper; ``fvm`` produces implicit matrix terms that can be summed into
an equation and solved.

This example builds a mesh from ``examples/case/``, reads ``U`` and
``p_rgh``, evaluates every ``fvc`` operator the solver API exposes, and
assembles a momentum matrix with ``fvm``.
"""

# %%
# Prepare a working case
# ----------------------
# The ``examples/`` tree is the documentation baseline and must stay
# pristine — clone the case into a tmp directory, restore ``0/`` from
# ``0.orig/``, then build the mesh inside the tmp copy.

import shutil
import tempfile
from pathlib import Path

import pybFoam.pybFoam_core as core
from pybFoam import meshing


def _examples_root() -> Path:
    for p in [Path.cwd(), *Path.cwd().parents]:
        if p.name == "examples":
            return p
    raise RuntimeError("Could not locate examples/ root")


BASELINE = (_examples_root() / "case").resolve()
CASE = Path(tempfile.mkdtemp(prefix="pybfoam_fvc_fvm_")) / "case"
shutil.copytree(BASELINE, CASE)

zero = CASE / "0"
if zero.exists():
    shutil.rmtree(zero)
shutil.copytree(CASE / "0.orig", zero)

args = core.argList([str(CASE), "-case", str(CASE)])
time = core.Time(args)

block_mesh_dict = core.dictionary.read(f"{CASE}/system/blockMeshDict")
mesh = meshing.generate_blockmesh(time, block_mesh_dict)
print(f"mesh: {mesh.nCells()} cells")

# %%
# Read fields
# -----------

from pybFoam import createPhi, volScalarField, volVectorField

p_rgh = volScalarField.read_field(mesh, "p_rgh")
U = volVectorField.read_field(mesh, "U")
phi = createPhi(U)

# %%
# Explicit operators (``fvc``)
# ----------------------------
# Each ``fvc`` call returns a ``tmp_*`` reference so OpenFOAM can reuse
# the underlying buffer — calling it (``()``) materialises the concrete
# field. Pass ``tmp_*`` directly into another operator to chain.

import numpy as np

from pybFoam import fvc

grad_p = fvc.grad(p_rgh)()
div_U = fvc.div(U)()
lap_p = fvc.laplacian(p_rgh)()

print(f"grad(p_rgh) shape  : {np.asarray(grad_p['internalField']).shape}")
print(
    f"div(U)      min/max: "
    f"{np.asarray(div_U['internalField']).min():.3e} / "
    f"{np.asarray(div_U['internalField']).max():.3e}"
)
print(f"lap(p_rgh)  sum    : {np.asarray(lap_p['internalField']).sum():.3e}")

# %%
# Flux + surface-normal gradient + reconstruct
# --------------------------------------------

flux = fvc.flux(U)()  # surfaceScalarField
div_phiU = fvc.div(flux, U)()
snGrad_p = fvc.snGrad(p_rgh)()  # surface-normal gradient
reconstructed = fvc.reconstruct(flux)()  # flux -> volVectorField

flux_vals = np.asarray(flux["internalField"])
snGrad_vals = np.asarray(snGrad_p["internalField"])
recon_vals = np.asarray(reconstructed["internalField"])

print(f"flux         : surfaceScalarField, {flux_vals.shape[0]} internal faces")
print(f"snGrad_p     : {snGrad_vals.shape[0]} internal face values")
print(f"reconstructed: volVectorField, {recon_vals.shape}")

# %%
# Implicit/matrix operators (``fvm``)
# -----------------------------------
# ``fvm`` operators produce terms of an equation matrix rather than
# field values. Combine them with ``+`` / ``-`` and wrap into an
# ``fvScalarMatrix`` or ``fvVectorMatrix``.

from pybFoam import fvm, fvScalarMatrix, fvVectorMatrix

# Scalar Laplacian: a single-term pressure Poisson matrix
pEqn = fvScalarMatrix(fvm.laplacian(p_rgh))

# Vector momentum: ddt + convection - diffusion (using constant nu=1e-5)
from pybFoam import Word, dimensionedScalar, dimLength, dimTime

nu = dimensionedScalar(Word("nu"), dimLength * dimLength / dimTime, 1.0e-5)
UEqn = fvVectorMatrix(fvm.ddt(U) + fvm.div(phi, U) - fvm.laplacian(nu, U))

print(f"pEqn: {type(pEqn).__name__}")
print(f"UEqn: {type(UEqn).__name__}")

# %%
# Mixing implicit and explicit terms
# ----------------------------------
# A typical PISO/SIMPLE loop builds the momentum matrix with ``fvm``,
# adds an explicit pressure gradient from ``fvc``, and hands the system
# to ``solve``:
#
# .. code-block:: python
#
#    if piso.momentumPredictor():
#        solve(UEqn + fvc.grad(p))
#
# The shipped case uses ``p_rgh`` (full pressure) whose dimensions do
# not match ``UEqn``, so we stop at showing the matrix shapes. For a
# worked solver see ``examples/cavity/icoFoam.py``.
