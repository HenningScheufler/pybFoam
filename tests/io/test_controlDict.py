import numpy as np
from pydantic import Field, create_model
from pybFoam.io.model_base import IOModelBase
from pybFoam.io.system import ControlDictBase
from pybFoam import Word
import pytest
import os
from pybFoam import dictionary

@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)


controlDict = create_model(
    'ControlDict',
    maxCo=(float, Field(..., gt=0.0)),  # name=(type, required/Default)
    test_token=(str, Field(..., description="A test token for demonstration purposes")),
    __base__=ControlDictBase
)

def test_parse_controlDict(change_test_dir):
    model = controlDict.from_file("controlDict")
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
    assert model.maxCo == 0.5  # Check the new field
    assert model.test_token == "token token"  # Check the new field

