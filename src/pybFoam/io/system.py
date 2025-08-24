from .model_base import IOModelBase
from pydantic import Field
from typing import Optional, Literal


class ControlDictBase(IOModelBase):
    application: str
    startFrom: Literal["startTime", "latestTime", "firstTime"] = Field(
        ...,
        description="Start time for the simulation, can be 'startTime', 'latestTime' or 'firstTime'",
    )
    startTime: float
    stopAt: Literal["endTime", "writeNow", "noWriteNow", "nextWrite"] = Field(
        ...,
        description="Stop condition for the simulation, can be 'endTime', 'writeNow', 'nextWrite' or 'noWriteNow'",
    )
    endTime: float
    deltaT: float
    writeControl: Literal[
        "timeStep", "runTime", "adjustable", "adjustableRunTime", "clockTime", "cpuTime"
    ] = Field(
        ...,
        description="Control for writing output, can be 'timeStep', 'runTime', 'adjustable', 'adjustableRunTime', 'clockTime' or 'cpuTime'",
    )
    writeInterval: float
    purgeWrite: int
    writeFormat: Literal["ascii", "binary"] = Field(
        ..., description="Format for writing files, must be 'ascii' or 'binary'"
    )
    writePrecision: int
    writeCompression: str
    timeFormat: str
    timePrecision: int
    runTimeModifiable: bool = Field(default=True)  # Default to True if not specified


class DDTSchemes(IOModelBase):
    default: Optional[str]

class GradSchemes(IOModelBase):
    default: Optional[str]

class DIVSchemes(IOModelBase):
    default: Optional[str]

class LaplacianSchemes(IOModelBase):
    default: Optional[str]

class InterpolationSchemes(IOModelBase):
    default: Optional[str]

class SnGradSchemes(IOModelBase):
    default: Optional[str]

class FluxRequired(IOModelBase):
    default: Optional[str]

class FvSchemesBase(IOModelBase):
    ddtSchemes: DDTSchemes
    gradSchemes: GradSchemes
    divSchemes: DIVSchemes
    laplacianSchemes: LaplacianSchemes
    interpolationSchemes: InterpolationSchemes
    snGradSchemes: SnGradSchemes
