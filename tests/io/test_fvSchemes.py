from typing import Optional
import pytest
from pybFoam.io.model_base import IOModelBase
from pybFoam.io.system import FvSchemesBase, DIVSchemes
import os
from pydantic import Field, create_model

@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)

ExtendedDIVSchemes = create_model(
    'ExtendedDIVSchemes',
    div_phi_U=(Optional[str], Field(alias="div(phi,U)", default=None)),
    div_phi_alpha=(Optional[str], Field(alias="div(phi,alpha)", default=None)),
    __base__=DIVSchemes  # Extend the original DIVSchemes
)

class FluxRequired(IOModelBase):
    default: Optional[str] = Field(default="yes", description="Indicates if fluxes are required, default is 'yes'")

# Create the complete FvSchemes model with the extended DIVSchemes
FvSchemes = create_model(
    'FvSchemes',
    divSchemes=(ExtendedDIVSchemes, ...),  # Override the divSchemes field,
    fluxRequired=(FluxRequired, ...),  # Add fluxRequired field
    __base__=FvSchemesBase
)

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


