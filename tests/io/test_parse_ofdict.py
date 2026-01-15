from typing import Optional
import pytest
from pybFoam import dictionary, vector, tensor, Word
import os
import numpy as np
from pydantic import Field
import pybFoam
from pybFoam.io.model_base import IOModelBase

@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)

class OFTestSubDict(IOModelBase):

    word2: Word

class OFTestDict(IOModelBase):
    
    word: Word = Field(json_schema_extra={"equals": "word"})
    scalar: float
    vector: vector
    tensor: tensor
    wordList: pybFoam.wordList = Field(max_length=2)
    scalarField: pybFoam.scalarField = Field(max_length=2)
    vectorField: pybFoam.vectorField = Field(max_length=2)
    tensorField: pybFoam.tensorField = Field(max_length=2)
    subDict: OFTestSubDict
    token: str
    optional: Optional[str] = None  # Optional field for testing
    notSetOptional: Optional[str] = None  # Not set, should be None


def test_parse_ofdict(change_test_dir):

    d = dictionary.read("TestDict")

    l_word = d.toc()
    assert l_word.list()[0] == "FoamFile"

    assert d.get[Word]("word") == "word"
    assert d.get[float]("scalar") == 1.1
    assert d.get[vector]("vector") == vector(1.1,1.1,1.1)
    assert d.get[tensor]("tensor") == tensor(1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1)

    # fields (now using proxy get interface)
    assert d.get[pybFoam.wordList]("wordList").list() == ["word1", "word2"]
    assert (d.get[pybFoam.scalarField]("scalarField") == np.ones(2)).all()
    assert (d.get[pybFoam.vectorField]("vectorField") == np.ones([2,3])).all()
    assert (d.get[pybFoam.tensorField]("tensorField") == np.ones([2,9])).all()

    subDict = d.subDict("subDict")
    assert subDict.get[Word]("word2") == "word2"

    assert d.get[str]("token") == "Gauss linear"
    assert d.get[str]("optional") == "optional"
    with pytest.raises(KeyError, match=r"Key \'notSetOptional\' not found in dictionary"):
        d.get[str]("notSetOptional")


import pytest

@pytest.mark.parametrize("filename", ["TestDict", "TestDict.yaml", "TestDict.json"])
def test_parse_ofdict_model(change_test_dir, filename):
    test_dict = OFTestDict.from_file(filename)
    assert test_dict.word == "word"
    assert test_dict.scalar == 1.1
    assert test_dict.vector == vector(1.1,1.1,1.1)
    assert test_dict.tensor == tensor(1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1)
    assert test_dict.wordList.list() == ["word1", "word2"]
    assert (test_dict.scalarField == np.ones(2)).all()
    assert (test_dict.vectorField == np.ones([2,3])).all()
    assert (test_dict.tensorField == np.ones([2,9])).all()
    assert test_dict.subDict.word2 == "word2"
    assert test_dict.token == "Gauss linear"
    assert test_dict.optional == "optional"
    assert test_dict.notSetOptional is None

class randomClass:
    pass

def test_exception(change_test_dir):

    d = dictionary.read("controlDict")

    with pytest.raises(RuntimeError, match=r"Unsupported type .*randomClass"):
        d.get[randomClass]("application")

    assert d.get[Word]("application") == "icoFoam"
    assert d.get[str]("application") == "icoFoam"


def test_getOrDefault(change_test_dir):
    """Test the dictionary.getOrDefault() method"""
    d = dictionary.read("TestDict")
    
    # Test getting existing keys returns actual values
    assert d.getOrDefault[Word]("word", "default") == "word"
    assert d.getOrDefault[Word]("not_excist_word", "default") == "default"
    assert d.getOrDefault[float]("scalar", 999.9) == 1.1
    assert d.getOrDefault[vector]("vector", vector(0,0,0)) == vector(1.1,1.1,1.1)
    
    # Test getting non-existing keys returns default values
    assert d.getOrDefault[str]("nonExistent", "myDefault") == "myDefault"
    assert d.getOrDefault[float]("missingScalar", 42.0) == 42.0
    assert d.getOrDefault[int]("missingInt", 123) == 123
    assert d.getOrDefault[Word]("missingWord", Word("defaultWord")) == "defaultWord"
    
    # Test with None as default
    assert d.getOrDefault[str]("notSetOptional", None) is None
    
    # Test in subdictionary
    subDict = d.subDict("subDict")
    assert subDict.getOrDefault[Word]("word2", "fallback") == "word2"
    assert subDict.getOrDefault[str]("missing", "fallback") == "fallback"
    
    # Test with complex types
    default_tensor = tensor(0,0,0,0,0,0,0,0,0)
    assert d.getOrDefault[tensor]("tensor", default_tensor) == tensor(1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1)
    assert d.getOrDefault[tensor]("missingTensor", default_tensor) == default_tensor
