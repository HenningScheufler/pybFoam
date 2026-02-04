"""
finite volume calculus
"""

from __future__ import annotations

import typing

import pybFoam.pybFoam_core

__all__: list[str] = [
    "ddtCorr",
    "div",
    "flux",
    "grad",
    "interpolate",
    "laplacian",
    "reconstruct",
    "snGrad",
]

def ddtCorr(
    arg0: pybFoam.pybFoam_core.volVectorField, arg1: pybFoam.pybFoam_core.surfaceScalarField
) -> pybFoam.pybFoam_core.tmp_surfaceScalarField: ...
@typing.overload
def div(arg0: pybFoam.pybFoam_core.volVectorField) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.tmp_volVectorField,
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def div(arg0: pybFoam.pybFoam_core.volTensorField) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.tmp_volTensorField,
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.volSymmTensorField,
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.tmp_volSymmTensorField,
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.surfaceScalarField,
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.tmp_surfaceScalarField,
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.surfaceVectorField,
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.tmp_surfaceVectorField,
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.surfaceTensorField,
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.tmp_surfaceTensorField,
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.surfaceSymmTensorField,
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.tmp_surfaceSymmTensorField,
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.volVectorField
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.tmp_volVectorField
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.volTensorField
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.tmp_volTensorField
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.volSymmTensorField
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def div(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.tmp_volSymmTensorField
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def flux(
    arg0: pybFoam.pybFoam_core.volVectorField,
) -> pybFoam.pybFoam_core.tmp_surfaceScalarField: ...
@typing.overload
def flux(
    arg0: pybFoam.pybFoam_core.tmp_volVectorField,
) -> pybFoam.pybFoam_core.tmp_surfaceScalarField: ...
@typing.overload
def flux(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.volVectorField
) -> pybFoam.pybFoam_core.tmp_surfaceVectorField: ...
@typing.overload
def flux(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.tmp_volVectorField
) -> pybFoam.pybFoam_core.tmp_surfaceVectorField: ...
@typing.overload
def grad(arg0: pybFoam.pybFoam_core.volScalarField) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def grad(
    arg0: pybFoam.pybFoam_core.tmp_volScalarField,
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def grad(arg0: pybFoam.pybFoam_core.volVectorField) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def grad(
    arg0: pybFoam.pybFoam_core.tmp_volVectorField,
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def grad(
    arg0: pybFoam.pybFoam_core.surfaceScalarField,
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def grad(
    arg0: pybFoam.pybFoam_core.tmp_surfaceScalarField,
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def grad(
    arg0: pybFoam.pybFoam_core.surfaceVectorField,
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def grad(
    arg0: pybFoam.pybFoam_core.tmp_surfaceVectorField,
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def interpolate(
    arg0: pybFoam.pybFoam_core.volScalarField,
) -> pybFoam.pybFoam_core.tmp_surfaceScalarField: ...
@typing.overload
def interpolate(
    arg0: pybFoam.pybFoam_core.tmp_volScalarField,
) -> pybFoam.pybFoam_core.tmp_surfaceScalarField: ...
@typing.overload
def interpolate(
    arg0: pybFoam.pybFoam_core.volVectorField,
) -> pybFoam.pybFoam_core.tmp_surfaceVectorField: ...
@typing.overload
def interpolate(
    arg0: pybFoam.pybFoam_core.tmp_volVectorField,
) -> pybFoam.pybFoam_core.tmp_surfaceVectorField: ...
@typing.overload
def interpolate(
    arg0: pybFoam.pybFoam_core.volTensorField,
) -> pybFoam.pybFoam_core.tmp_surfaceTensorField: ...
@typing.overload
def interpolate(
    arg0: pybFoam.pybFoam_core.tmp_volTensorField,
) -> pybFoam.pybFoam_core.tmp_surfaceTensorField: ...
@typing.overload
def interpolate(
    arg0: pybFoam.pybFoam_core.volSymmTensorField,
) -> pybFoam.pybFoam_core.tmp_surfaceSymmTensorField: ...
@typing.overload
def interpolate(
    arg0: pybFoam.pybFoam_core.tmp_volSymmTensorField,
) -> pybFoam.pybFoam_core.tmp_surfaceSymmTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volScalarField,
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volScalarField,
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volVectorField,
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volVectorField,
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volTensorField,
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volTensorField,
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volSymmTensorField,
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volSymmTensorField,
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volScalarField, arg1: pybFoam.pybFoam_core.volScalarField
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volScalarField, arg1: pybFoam.pybFoam_core.tmp_volScalarField
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volScalarField, arg1: pybFoam.pybFoam_core.volScalarField
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volScalarField, arg1: pybFoam.pybFoam_core.tmp_volScalarField
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volScalarField, arg1: pybFoam.pybFoam_core.volVectorField
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volScalarField, arg1: pybFoam.pybFoam_core.tmp_volVectorField
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volScalarField, arg1: pybFoam.pybFoam_core.volVectorField
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volScalarField, arg1: pybFoam.pybFoam_core.tmp_volVectorField
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volScalarField, arg1: pybFoam.pybFoam_core.volTensorField
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volScalarField, arg1: pybFoam.pybFoam_core.tmp_volTensorField
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volScalarField, arg1: pybFoam.pybFoam_core.volTensorField
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volScalarField, arg1: pybFoam.pybFoam_core.tmp_volTensorField
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volScalarField, arg1: pybFoam.pybFoam_core.volSymmTensorField
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volScalarField, arg1: pybFoam.pybFoam_core.tmp_volSymmTensorField
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volScalarField, arg1: pybFoam.pybFoam_core.volSymmTensorField
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volScalarField, arg1: pybFoam.pybFoam_core.tmp_volSymmTensorField
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.volScalarField
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.tmp_volScalarField
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_surfaceScalarField, arg1: pybFoam.pybFoam_core.volScalarField
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_surfaceScalarField, arg1: pybFoam.pybFoam_core.tmp_volScalarField
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.volVectorField
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.tmp_volVectorField
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_surfaceScalarField, arg1: pybFoam.pybFoam_core.volVectorField
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_surfaceScalarField, arg1: pybFoam.pybFoam_core.tmp_volVectorField
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.volTensorField
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.tmp_volTensorField
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_surfaceScalarField, arg1: pybFoam.pybFoam_core.volTensorField
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_surfaceScalarField, arg1: pybFoam.pybFoam_core.tmp_volTensorField
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.volSymmTensorField
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.surfaceScalarField, arg1: pybFoam.pybFoam_core.tmp_volSymmTensorField
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_surfaceScalarField, arg1: pybFoam.pybFoam_core.volSymmTensorField
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_surfaceScalarField,
    arg1: pybFoam.pybFoam_core.tmp_volSymmTensorField,
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volTensorField, arg1: pybFoam.pybFoam_core.volScalarField
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volTensorField, arg1: pybFoam.pybFoam_core.tmp_volScalarField
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volTensorField, arg1: pybFoam.pybFoam_core.volScalarField
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volTensorField, arg1: pybFoam.pybFoam_core.tmp_volScalarField
) -> pybFoam.pybFoam_core.tmp_volScalarField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volTensorField, arg1: pybFoam.pybFoam_core.volVectorField
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volTensorField, arg1: pybFoam.pybFoam_core.tmp_volVectorField
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volTensorField, arg1: pybFoam.pybFoam_core.volVectorField
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volTensorField, arg1: pybFoam.pybFoam_core.tmp_volVectorField
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volTensorField, arg1: pybFoam.pybFoam_core.volTensorField
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volTensorField, arg1: pybFoam.pybFoam_core.tmp_volTensorField
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volTensorField, arg1: pybFoam.pybFoam_core.volTensorField
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volTensorField, arg1: pybFoam.pybFoam_core.tmp_volTensorField
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volTensorField, arg1: pybFoam.pybFoam_core.volSymmTensorField
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.volTensorField, arg1: pybFoam.pybFoam_core.tmp_volSymmTensorField
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volTensorField, arg1: pybFoam.pybFoam_core.volSymmTensorField
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def laplacian(
    arg0: pybFoam.pybFoam_core.tmp_volTensorField, arg1: pybFoam.pybFoam_core.tmp_volSymmTensorField
) -> pybFoam.pybFoam_core.tmp_volSymmTensorField: ...
@typing.overload
def reconstruct(
    arg0: pybFoam.pybFoam_core.surfaceScalarField,
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def reconstruct(
    arg0: pybFoam.pybFoam_core.tmp_surfaceScalarField,
) -> pybFoam.pybFoam_core.tmp_volVectorField: ...
@typing.overload
def reconstruct(
    arg0: pybFoam.pybFoam_core.surfaceVectorField,
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def reconstruct(
    arg0: pybFoam.pybFoam_core.tmp_surfaceVectorField,
) -> pybFoam.pybFoam_core.tmp_volTensorField: ...
@typing.overload
def snGrad(
    arg0: pybFoam.pybFoam_core.volScalarField,
) -> pybFoam.pybFoam_core.tmp_surfaceScalarField: ...
@typing.overload
def snGrad(
    arg0: pybFoam.pybFoam_core.tmp_volScalarField,
) -> pybFoam.pybFoam_core.tmp_surfaceScalarField: ...
@typing.overload
def snGrad(
    arg0: pybFoam.pybFoam_core.volVectorField,
) -> pybFoam.pybFoam_core.tmp_surfaceVectorField: ...
@typing.overload
def snGrad(
    arg0: pybFoam.pybFoam_core.tmp_volVectorField,
) -> pybFoam.pybFoam_core.tmp_surfaceVectorField: ...
