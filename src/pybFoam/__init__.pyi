from __future__ import annotations
from pybFoam.pybFoam_core import DictionaryGetOrDefaultProxy
from pybFoam.pybFoam_core import DictionaryGetProxy
from pybFoam.pybFoam_core import Info
from pybFoam.pybFoam_core import SolverScalarPerformance
from pybFoam.pybFoam_core import SolverSymmTensorPerformance
from pybFoam.pybFoam_core import SolverTensorPerformance
from pybFoam.pybFoam_core import SolverVectorPerformance
from pybFoam.pybFoam_core import SymmTensorInt
from pybFoam.pybFoam_core import TensorInt
from pybFoam.pybFoam_core import Time
from pybFoam.pybFoam_core import VectorInt
from pybFoam.pybFoam_core import Word
from pybFoam.pybFoam_core import adjustPhi
from pybFoam.pybFoam_core import argList
from pybFoam.pybFoam_core import boolList
from pybFoam.pybFoam_core import computeCFLNumber
from pybFoam.pybFoam_core import computeContinuityErrors
from pybFoam.pybFoam_core import constrainHbyA
from pybFoam.pybFoam_core import constrainPressure
from pybFoam.pybFoam_core import createMesh
from pybFoam.pybFoam_core import createPhi
from pybFoam.pybFoam_core import dictionary
from pybFoam.pybFoam_core import dimensionSet
from pybFoam.pybFoam_core import dimensionedScalar
from pybFoam.pybFoam_core import dimensionedSymmTensor
from pybFoam.pybFoam_core import dimensionedTensor
from pybFoam.pybFoam_core import dimensionedVector
from pybFoam.pybFoam_core import dynamicFvMesh
from pybFoam.pybFoam_core import entry
from pybFoam.pybFoam_core import fvMesh
from pybFoam.pybFoam_core import fvScalarMatrix
from pybFoam.pybFoam_core import fvSymmTensorMatrix
from pybFoam.pybFoam_core import fvTensorMatrix
from pybFoam.pybFoam_core import fvVectorMatrix
from pybFoam.pybFoam_core import instant
from pybFoam.pybFoam_core import instantList
from pybFoam.pybFoam_core import keyType
from pybFoam.pybFoam_core import labelList
from pybFoam.pybFoam_core import mag
from pybFoam.pybFoam_core import pimpleControl
from pybFoam.pybFoam_core import pisoControl
from pybFoam.pybFoam_core import scalarField
from pybFoam.pybFoam_core import selectTimes
from pybFoam.pybFoam_core import setRefCell
from pybFoam.pybFoam_core import simpleControl
from pybFoam.pybFoam_core import solve
from pybFoam.pybFoam_core import sum
from pybFoam.pybFoam_core import surfaceScalarField
from pybFoam.pybFoam_core import surfaceSymmTensorField
from pybFoam.pybFoam_core import surfaceTensorField
from pybFoam.pybFoam_core import surfaceVectorField
from pybFoam.pybFoam_core import symmTensor
from pybFoam.pybFoam_core import symmTensorField
from pybFoam.pybFoam_core import tensor
from pybFoam.pybFoam_core import tensorField
from pybFoam.pybFoam_core import tmp_fvScalarMatrix
from pybFoam.pybFoam_core import tmp_fvSymmTensorMatrix
from pybFoam.pybFoam_core import tmp_fvTensorMatrix
from pybFoam.pybFoam_core import tmp_fvVectorMatrix
from pybFoam.pybFoam_core import tmp_scalarField
from pybFoam.pybFoam_core import tmp_surfaceScalarField
from pybFoam.pybFoam_core import tmp_surfaceSymmTensorField
from pybFoam.pybFoam_core import tmp_surfaceTensorField
from pybFoam.pybFoam_core import tmp_surfaceVectorField
from pybFoam.pybFoam_core import tmp_symmTensorField
from pybFoam.pybFoam_core import tmp_tensorField
from pybFoam.pybFoam_core import tmp_vectorField
from pybFoam.pybFoam_core import tmp_volScalarField
from pybFoam.pybFoam_core import tmp_volSymmTensorField
from pybFoam.pybFoam_core import tmp_volTensorField
from pybFoam.pybFoam_core import tmp_volVectorField
from pybFoam.pybFoam_core import uniformDimensionedScalarField
from pybFoam.pybFoam_core import uniformDimensionedVectorField
from pybFoam.pybFoam_core import vector
from pybFoam.pybFoam_core import vectorField
from pybFoam.pybFoam_core import volScalarField
from pybFoam.pybFoam_core import volSymmTensorField
from pybFoam.pybFoam_core import volTensorField
from pybFoam.pybFoam_core import volVectorField
from pybFoam.pybFoam_core import wordList
from pybFoam.pybFoam_core import write
from . import _version
from . import fvc
from . import fvm
from . import pybFoam_core
from . import runTimeTables
from . import sampling_bindings
from . import thermo
from . import turbulence
__all__: list[str] = ['DictionaryGetOrDefaultProxy', 'DictionaryGetProxy', 'Info', 'SolverScalarPerformance', 'SolverSymmTensorPerformance', 'SolverTensorPerformance', 'SolverVectorPerformance', 'SymmTensorInt', 'TensorInt', 'Time', 'VectorInt', 'Word', 'adjustPhi', 'argList', 'boolList', 'computeCFLNumber', 'computeContinuityErrors', 'constrainHbyA', 'constrainPressure', 'createMesh', 'createPhi', 'dictionary', 'dimAcceleration', 'dimArea', 'dimCurrent', 'dimDensity', 'dimEnergy', 'dimForce', 'dimLength', 'dimLuminousIntensity', 'dimMass', 'dimMoles', 'dimPower', 'dimPressure', 'dimTemperature', 'dimTime', 'dimVelocity', 'dimViscosity', 'dimensionSet', 'dimensionedScalar', 'dimensionedSymmTensor', 'dimensionedTensor', 'dimensionedVector', 'dimless', 'dynamicFvMesh', 'entry', 'fvMesh', 'fvScalarMatrix', 'fvSymmTensorMatrix', 'fvTensorMatrix', 'fvVectorMatrix', 'fvc', 'fvm', 'instant', 'instantList', 'keyType', 'labelList', 'mag', 'pimpleControl', 'pisoControl', 'pybFoam_core', 'runTimeTables', 'sampling_bindings', 'scalarField', 'selectTimes', 'setRefCell', 'simpleControl', 'solve', 'sum', 'surfaceScalarField', 'surfaceSymmTensorField', 'surfaceTensorField', 'surfaceVectorField', 'symmTensor', 'symmTensorField', 'tensor', 'tensorField', 'thermo', 'tmp_fvScalarMatrix', 'tmp_fvSymmTensorMatrix', 'tmp_fvTensorMatrix', 'tmp_fvVectorMatrix', 'tmp_scalarField', 'tmp_surfaceScalarField', 'tmp_surfaceSymmTensorField', 'tmp_surfaceTensorField', 'tmp_surfaceVectorField', 'tmp_symmTensorField', 'tmp_tensorField', 'tmp_vectorField', 'tmp_volScalarField', 'tmp_volSymmTensorField', 'tmp_volTensorField', 'tmp_volVectorField', 'turbulence', 'uniformDimensionedScalarField', 'uniformDimensionedVectorField', 'vector', 'vectorField', 'volScalarField', 'volSymmTensorField', 'volTensorField', 'volVectorField', 'wordList', 'write']
__version__: str = '0.1.10'
dimAcceleration: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimArea: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimCurrent: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimDensity: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimEnergy: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimForce: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimLength: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimLuminousIntensity: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimMass: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimMoles: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimPower: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimPressure: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimTemperature: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimTime: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimVelocity: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimViscosity: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
dimless: pybFoam_core.dimensionSet  # value = <pybFoam.pybFoam_core.dimensionSet object>
