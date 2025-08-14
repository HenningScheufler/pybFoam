from typing import Optional
import pytest
from pybFoam.io.model_base import IOModelBase
import os
from pydantic import Field

@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)

class DDTSchemes(IOModelBase):
    default: Optional[str]

class GradSchemes(IOModelBase):
    default: Optional[str]

class DIVSchemes(IOModelBase):
    default: Optional[str]
    div_phi_U: str = Field(alias="div(phi,U)")
    div_phi_alpha: str = Field(alias="div(phi,alpha)")

class LaplacianSchemes(IOModelBase):
    default: Optional[str]

class InterpolationSchemes(IOModelBase):
    default: Optional[str]

class SnGradSchemes(IOModelBase):
    default: Optional[str]

class FluxRequired(IOModelBase):
    default: Optional[str]

class FvSchemes(IOModelBase):
    ddtSchemes: DDTSchemes
    gradSchemes: GradSchemes
    divSchemes: DIVSchemes
    laplacianSchemes: LaplacianSchemes
    interpolationSchemes: InterpolationSchemes
    snGradSchemes: SnGradSchemes
    fluxRequired: FluxRequired

def test_parse_fvSchemes(change_test_dir):
    model = FvSchemes.from_file("fvSchemes")
    assert model.ddtSchemes.default == "Euler"

    assert model.gradSchemes.default == "Gauss linear"

    assert model.divSchemes.default == "none"
    assert model.divSchemes.div_phi_U == "Gauss limitedLinearV 1"
    assert model.divSchemes.div_phi_alpha == "Gauss linear"

    assert model.laplacianSchemes.default == "Gauss linear corrected"

    assert model.interpolationSchemes.default == "linear"

    assert model.snGradSchemes.default == "corrected"

    assert model.fluxRequired.default == "yes"


