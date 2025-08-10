import numpy as np
from pydantic import Field
from pybFoam.io.model_base import IOModelBase
from pybFoam import Word
import pytest
import os
from pybFoam import dictionary

@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)



class ControlDict(IOModelBase):
    application: str
    startFrom: str
    startTime: float
    stopAt: str
    endTime: float
    deltaT: float
    writeControl: str
    writeInterval: float
    purgeWrite: int
    writeFormat: str
    writePrecision: int
    writeCompression: str
    timeFormat: str
    timePrecision: int
    runTimeModifiable: bool


def test_parse_controlDict(change_test_dir):
    model = ControlDict.from_file("controlDict")
    assert model.application == "icoFoam"
    assert model.startFrom == "startTime"
    assert model.startTime == 0
    assert model.stopAt == "endTime"
    assert model.endTime == 10
    assert model.deltaT == 0.05
    assert model.writeControl == "timeStep"
    assert model.writeInterval == 20
    assert model.purgeWrite == 0
    assert model.writeFormat == "ascii"
    assert model.writePrecision == 6
    assert model.writeCompression == "off"
    assert model.timeFormat == "general"
    assert model.timePrecision == 6
    assert model.runTimeModifiable is True


