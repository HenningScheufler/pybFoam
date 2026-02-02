from __future__ import annotations

from pybFoam.pybFoam_core import (
    DictionaryGetOrDefaultProxy,
    DictionaryGetProxy,
    Info,
    IOobject,
    SolverScalarPerformance,
    SolverSymmTensorPerformance,
    SolverTensorPerformance,
    SolverVectorPerformance,
    SymmTensorInt,
    TensorInt,
    Time,
    VectorInt,
    Word,
    adjustPhi,
    argList,
    boolList,
    computeCFLNumber,
    computeContinuityErrors,
    constrainHbyA,
    constrainPressure,
    createMesh,
    createPhi,
    dictionary,
    dimensionedScalar,
    dimensionedSymmTensor,
    dimensionedTensor,
    dimensionedVector,
    dimensionSet,
    dynamicFvMesh,
    entry,
    fileName,
    fvMesh,
    fvScalarMatrix,
    fvSymmTensorMatrix,
    fvTensorMatrix,
    fvVectorMatrix,
    instant,
    instantList,
    keyType,
    labelList,
    mag,
    pimpleControl,
    pisoControl,
    polyBoundaryMesh,
    polyMesh,
    polyPatch,
    scalarField,
    selectTimes,
    setRefCell,
    simpleControl,
    solve,
    sum,
    surfaceScalarField,
    surfaceSymmTensorField,
    surfaceTensorField,
    surfaceVectorField,
    symmTensor,
    symmTensorField,
    tensor,
    tensorField,
    tmp_fvScalarMatrix,
    tmp_fvSymmTensorMatrix,
    tmp_fvTensorMatrix,
    tmp_fvVectorMatrix,
    tmp_scalarField,
    tmp_surfaceScalarField,
    tmp_surfaceSymmTensorField,
    tmp_surfaceTensorField,
    tmp_surfaceVectorField,
    tmp_symmTensorField,
    tmp_tensorField,
    tmp_vectorField,
    tmp_volScalarField,
    tmp_volSymmTensorField,
    tmp_volTensorField,
    tmp_volVectorField,
    uniformDimensionedScalarField,
    uniformDimensionedVectorField,
    vector,
    vectorField,
    volScalarField,
    volSymmTensorField,
    volTensorField,
    volVectorField,
    wordList,
    write,
)

from . import (
    fvc,
    fvm,
    meshing,
    pybFoam_core,
    runTimeTables,
    sampling_bindings,
    thermo,
    turbulence,
)

__all__: list[str] = ['DictionaryGetOrDefaultProxy', 'DictionaryGetProxy', 'Info', 'IOobject', 'Time', 'Word', 'argList', 'dictionary', 'entry', 'fileName', 'instant', 'instantList', 'keyType', 'dynamicFvMesh', 'fvMesh', 'polyBoundaryMesh', 'polyMesh', 'polyPatch', 'SolverScalarPerformance', 'SolverSymmTensorPerformance', 'SolverTensorPerformance', 'SolverVectorPerformance', 'SymmTensorInt', 'TensorInt', 'VectorInt', 'boolList', 'labelList', 'wordList', 'symmTensor', 'tensor', 'vector', 'scalarField', 'symmTensorField', 'tensorField', 'vectorField', 'volScalarField', 'volSymmTensorField', 'volTensorField', 'volVectorField', 'surfaceScalarField', 'surfaceSymmTensorField', 'surfaceTensorField', 'surfaceVectorField', 'uniformDimensionedScalarField', 'uniformDimensionedVectorField', 'tmp_scalarField', 'tmp_symmTensorField', 'tmp_tensorField', 'tmp_vectorField', 'tmp_volScalarField', 'tmp_volSymmTensorField', 'tmp_volTensorField', 'tmp_volVectorField', 'tmp_surfaceScalarField', 'tmp_surfaceSymmTensorField', 'tmp_surfaceTensorField', 'tmp_surfaceVectorField', 'fvScalarMatrix', 'fvSymmTensorMatrix', 'fvTensorMatrix', 'fvVectorMatrix', 'tmp_fvScalarMatrix', 'tmp_fvSymmTensorMatrix', 'tmp_fvTensorMatrix', 'tmp_fvVectorMatrix', 'dimensionedScalar', 'dimensionedSymmTensor', 'dimensionedTensor', 'dimensionedVector', 'dimensionSet', 'dimAcceleration', 'dimArea', 'dimCurrent', 'dimDensity', 'dimEnergy', 'dimForce', 'dimLength', 'dimless', 'dimLuminousIntensity', 'dimMass', 'dimMoles', 'dimPower', 'dimPressure', 'dimTemperature', 'dimTime', 'dimVelocity', 'dimViscosity', 'pimpleControl', 'pisoControl', 'simpleControl', 'adjustPhi', 'computeCFLNumber', 'computeContinuityErrors', 'constrainHbyA', 'constrainPressure', 'createMesh', 'createPhi', 'mag', 'selectTimes', 'setRefCell', 'solve', 'sum', 'write', 'fvc', 'fvm', 'meshing', 'runTimeTables', 'sampling_bindings', 'thermo', 'turbulence', '__version__']
__version__: str = '0.1.11'
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
