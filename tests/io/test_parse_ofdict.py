import os
from typing import Any, Generator, Optional

import numpy as np
import pytest
from pydantic import Field

import pybFoam
from pybFoam import Word, dictionary, tensor, vector
from pybFoam.io.model_base import IOModelBase


@pytest.fixture(scope="function")
def change_test_dir(request: Any) -> Generator[None, None, None]:
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


def test_parse_ofdict(change_test_dir: Any) -> None:
    d = dictionary.read("TestDict")

    l_word = d.toc()
    assert l_word.list()[0] == "FoamFile"

    assert d.get[Word]("word") == "word"  # type: ignore[operator]
    assert d.get[float]("scalar") == 1.1  # type: ignore[operator]
    assert d.get[vector]("vector") == vector(1.1, 1.1, 1.1)  # type: ignore[operator]
    assert d.get[tensor]("tensor") == tensor(1.1, 1.1, 1.1, 1.1, 1.1, 1.1, 1.1, 1.1, 1.1)  # type: ignore[operator]

    # fields (now using proxy get interface)
    assert d.get[pybFoam.wordList]("wordList").list() == ["word1", "word2"]  # type: ignore[operator]
    assert (d.get[pybFoam.scalarField]("scalarField") == np.ones(2)).all()  # type: ignore[operator]
    assert (d.get[pybFoam.vectorField]("vectorField") == np.ones([2, 3])).all()  # type: ignore[operator]
    assert (d.get[pybFoam.tensorField]("tensorField") == np.ones([2, 9])).all()  # type: ignore[operator]

    subDict = d.subDict("subDict")
    assert subDict.get[Word]("word2") == "word2"  # type: ignore[operator]

    assert d.get[str]("token") == "Gauss linear"  # type: ignore[operator]
    assert d.get[str]("optional") == "optional"  # type: ignore[operator]
    with pytest.raises(KeyError, match=r"Key \'notSetOptional\' not found in dictionary"):
        d.get[str]("notSetOptional")  # type: ignore[operator]


@pytest.mark.parametrize("filename", ["TestDict", "TestDict.yaml", "TestDict.json"])
def test_parse_ofdict_model(change_test_dir: Any, filename: str) -> None:
    test_dict = OFTestDict.from_file(filename)
    assert test_dict.word == "word"
    assert test_dict.scalar == 1.1
    assert test_dict.vector == vector(1.1, 1.1, 1.1)
    assert test_dict.tensor == tensor(1.1, 1.1, 1.1, 1.1, 1.1, 1.1, 1.1, 1.1, 1.1)
    assert test_dict.wordList.list() == ["word1", "word2"]
    assert (test_dict.scalarField == np.ones(2)).all()
    assert (test_dict.vectorField == np.ones([2, 3])).all()
    assert (test_dict.tensorField == np.ones([2, 9])).all()
    assert test_dict.subDict.word2 == "word2"
    assert test_dict.token == "Gauss linear"
    assert test_dict.optional == "optional"
    assert test_dict.notSetOptional is None


class randomClass:
    pass


def test_exception(change_test_dir: Any) -> None:
    d = dictionary.read("controlDict")

    with pytest.raises(RuntimeError, match=r"Unsupported type .*randomClass"):
        d.get[randomClass]("application")  # type: ignore[operator]

    assert d.get[Word]("application") == "icoFoam"  # type: ignore[operator]
    assert d.get[str]("application") == "icoFoam"  # type: ignore[operator]


def test_getOrDefault(change_test_dir: Any) -> None:
    """Test the dictionary.getOrDefault() method"""
    d = dictionary.read("TestDict")

    # Test getting existing keys returns actual values
    assert d.getOrDefault[Word]("word", "default") == "word"  # type: ignore[operator]
    assert d.getOrDefault[Word]("not_excist_word", "default") == "default"  # type: ignore[operator]
    assert d.getOrDefault[float]("scalar", 999.9) == 1.1  # type: ignore[operator]
    assert d.getOrDefault[vector]("vector", vector(0, 0, 0)) == vector(1.1, 1.1, 1.1)  # type: ignore[operator]

    # Test getting non-existing keys returns default values
    assert d.getOrDefault[str]("nonExistent", "myDefault") == "myDefault"  # type: ignore[operator]
    assert d.getOrDefault[float]("missingScalar", 42.0) == 42.0  # type: ignore[operator]
    assert d.getOrDefault[int]("missingInt", 123) == 123  # type: ignore[operator]
    assert d.getOrDefault[Word]("missingWord", Word("defaultWord")) == "defaultWord"  # type: ignore[operator]

    # Test with None as default
    assert d.getOrDefault[str]("notSetOptional", None) is None  # type: ignore[operator]

    # Test in subdictionary
    subDict = d.subDict("subDict")
    assert subDict.getOrDefault[Word]("word2", "fallback") == "word2"  # type: ignore[operator]
    assert subDict.getOrDefault[str]("missing", "fallback") == "fallback"  # type: ignore[operator]

    # Test with complex types
    default_tensor = tensor(0, 0, 0, 0, 0, 0, 0, 0, 0)
    assert d.getOrDefault[tensor]("tensor", default_tensor) == tensor(  # type: ignore[operator]
        1.1, 1.1, 1.1, 1.1, 1.1, 1.1, 1.1, 1.1, 1.1
    )
    assert d.getOrDefault[tensor]("missingTensor", default_tensor) == default_tensor  # type: ignore[operator]
