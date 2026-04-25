"""
Getting started — open a case and read a field
===============================================

Your first five minutes with pybFoam: open an OpenFOAM case from
Python, build a mesh, and read a field into a NumPy array. Everything
else in this gallery builds on this pattern.

This tutorial runs against the minimal ``examples/case/`` — a VOF cavity
setup shipped with the repo. We clone the case into a tmp directory so
the baseline in ``examples/`` stays pristine.
"""

# %%
# Prerequisites
# -------------
#
# * OpenFOAM v2312+ installed and its environment sourced
#   (``source /path/to/OpenFOAM/etc/bashrc``).
# * pybFoam installed (``pip install pybFoam[all]``).
#
# If either is missing, every subsequent cell fails — there is no
# meaningful fallback. ``echo "$FOAM_SRC"`` should return a non-empty
# path.

# %%
# Clone the baseline case
# -----------------------
# Sphinx-gallery runs each script with ``cwd`` set to the script's own
# folder. We resolve ``examples/case/`` relative to that and copy the
# case into a tmp directory so the example is side-effect-free.

import shutil
import tempfile
from pathlib import Path


def _examples_root() -> Path:
    for p in [Path.cwd(), *Path.cwd().parents]:
        if p.name == "examples":
            return p
    raise RuntimeError("Could not locate examples/ root")


BASELINE = (_examples_root() / "case").resolve()
CASE = Path(tempfile.mkdtemp(prefix="pybfoam_getting_started_")) / "case"
shutil.copytree(BASELINE, CASE)
print(f"working case: {CASE}")

# %%
# Create ``Time`` and ``fvMesh``
# ------------------------------
# ``argList`` parses OpenFOAM-style command-line options. ``Time`` is
# the simulation clock. ``fvMesh`` loads (or in our case will generate)
# the finite-volume mesh.

import pybFoam.pybFoam_core as core
from pybFoam import meshing

args = core.argList([str(CASE), "-case", str(CASE)])
time = core.Time(args)

# %%
# Build the mesh
# --------------
# ``examples/case/`` ships with a ``blockMeshDict`` but no pre-built
# ``polyMesh/`` — call :func:`meshing.generate_blockmesh` to build one.
# The returned ``mesh`` is a regular :class:`fvMesh` from that point on.

block_mesh_dict = core.dictionary.read(f"{CASE}/system/blockMeshDict")
mesh = meshing.generate_blockmesh(time, block_mesh_dict)

print(f"nCells   = {mesh.nCells()}")
print(f"nPoints  = {mesh.nPoints()}")
print(f"nFaces   = {mesh.nFaces()}")

# %%
# Restore the ``0/`` fields
# -------------------------
# Field files live under ``0.orig/`` in the baseline case. Solvers
# normally use ``restore0Dir`` to copy them into ``0/`` before they
# start — we do the same by hand here.

zero, zero_orig = CASE / "0", CASE / "0.orig"
if zero.exists():
    shutil.rmtree(zero)
shutil.copytree(zero_orig, zero)

# %%
# Read a field
# ------------
# Any field listed under ``0/`` (or that was written by a previous
# solver run) can be read directly. :func:`volScalarField.read_field`
# and :func:`volVectorField.read_field` do the lookup.

import numpy as np

from pybFoam import volScalarField, volVectorField

p_rgh = volScalarField.read_field(mesh, "p_rgh")
U = volVectorField.read_field(mesh, "U")

# %%
# Zero-copy NumPy views
# ---------------------
# Indexing a field with ``["internalField"]`` gives a buffer; wrapping
# it in :func:`numpy.asarray` returns a NumPy view that shares memory
# with OpenFOAM — no copy, no conversion.

p_array = np.asarray(p_rgh["internalField"])
U_array = np.asarray(U["internalField"])

print(f"p_rgh shape: {p_array.shape}")
print(f"U shape    : {U_array.shape}")
print(f"p_rgh range: {p_array.min():.3g} … {p_array.max():.3g}")
print(f"|U| max    : {np.linalg.norm(U_array, axis=1).max():.3g}")

# %%
# What's next
# -----------
#
# * :doc:`example_02_field_analysis` — loop over time directories and
#   build a summary across a whole simulation.
# * :doc:`example_03_sampling_workflow` — extract values on a plane and
#   export them to CSV.
# * :doc:`/auto_how_to/fvc_fvm/example_fvc_fvm_operators` — compute
#   gradients, divergences, and Laplacians of the fields you just read.
