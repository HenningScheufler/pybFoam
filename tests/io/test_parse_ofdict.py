from pydantic import BaseModel
import pytest
from pybFoam import dictionary, vector, tensor, Word
import os
import numpy as np

import pybFoam

@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)

class OFTestSubDict(BaseModel):
    @classmethod
    def from_yaml(cls, y):
        return cls(word2=y["word2"])

    @classmethod
    def from_json(cls, j):
        return cls(word2=j["word2"])
    model_config = {"arbitrary_types_allowed": True}

    word2: Word

class OFTestDict(BaseModel):
    model_config = {"arbitrary_types_allowed": True}
    
    word: Word
    scalar: float
    vector: vector
    tensor: tensor
    wordList: pybFoam.wordList
    scalarField: pybFoam.scalarField
    vectorField: pybFoam.vectorField
    tensorField: pybFoam.tensorField
    subDict: OFTestSubDict
    @classmethod
    def from_ofdict(cls, d):
        return cls(
            word=d.get[Word]("word"),
            scalar=d.get[float]("scalar"),
            vector=d.get[vector]("vector"),
            tensor=d.get[tensor]("tensor"),
            wordList=d.get[pybFoam.wordList]("wordList"),
            scalarField=d.get[pybFoam.scalarField]("scalarField"),
            vectorField=d.get[pybFoam.vectorField]("vectorField"),
            tensorField=d.get[pybFoam.tensorField]("tensorField"),
            subDict=OFTestSubDict(word2=d.subDict("subDict").get[Word]("word2")) if d.isDict("subDict") else None
        )

    @classmethod
    def from_yaml(cls, y):
        return cls(
            word=y["word"],
            scalar=y["scalar"],
            vector=vector(*y["vector"]),
            tensor=tensor(*y["tensor"]),
            wordList=pybFoam.wordList(y["wordList"]),
            scalarField=pybFoam.scalarField(y["scalarField"]),
            vectorField=pybFoam.vectorField(y["vectorField"]),
            tensorField=pybFoam.tensorField(y["tensorField"]),
            subDict=OFTestSubDict.from_yaml(y["subDict"]) if "subDict" in y else None
        )

    @classmethod
    def from_json(cls, j):
        return cls(
            word=j["word"],
            scalar=j["scalar"],
            vector=vector(*j["vector"]),
            tensor=tensor(*j["tensor"]),
            wordList=pybFoam.wordList(j["wordList"]),
            scalarField=pybFoam.scalarField(j["scalarField"]),
            vectorField=pybFoam.vectorField(j["vectorField"]),
            tensorField=pybFoam.tensorField(j["tensorField"]),
            subDict=OFTestSubDict.from_json(j["subDict"]) if "subDict" in j else None
        )


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

    # assumes the other are also correct
    # d.set("word",Word("word2"))
    # assert d.get[Word]("word") == "word2"

    # d.set("wordList2",pybFoam.wordList(["word3","word4"]))
    # assert  d.get_wordList("wordList2").list() == ["word3", "word4"]

    subDict = d.subDict("subDict")
    assert subDict.get[Word]("word2") == "word2"


def test_parse_ofdict_model(change_test_dir):
    d = dictionary.read("TestDict")

    l_word = d.toc()
    assert l_word.list()[0] == "FoamFile"

    # primitives
    test_dict = OFTestDict.from_ofdict(d)
    assert test_dict.word == "word"
    assert test_dict.scalar == 1.1
    assert test_dict.vector == vector(1.1,1.1,1.1)
    assert test_dict.tensor == tensor(1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1)
    assert test_dict.wordList.list() == ["word1", "word2"]
    assert (test_dict.scalarField == np.ones(2)).all()
    assert (test_dict.vectorField == np.ones([2,3])).all()
    assert (test_dict.tensorField == np.ones([2,9])).all()
    assert test_dict.subDict.word2 == "word2"

    # YAML
    import yaml
    with open("TestDict.yaml") as f:
        ydata = yaml.safe_load(f)
    test_dict_yaml = OFTestDict.from_yaml(ydata)
    assert test_dict_yaml == test_dict

    # JSON
    import json
    with open("TestDict.json") as f:
        jdata = json.load(f)
    test_dict_json = OFTestDict.from_json(jdata)
    assert test_dict_json == test_dict

def test_parse_ofdict_model_yaml(change_test_dir):
    d = dictionary.read("TestDict")

    l_word = d.toc()
    assert l_word.list()[0] == "FoamFile"

    # primitives
    test_dict = OFTestDict.from_ofdict(d)
    assert test_dict.word == "word"
    assert test_dict.scalar == 1.1
    assert test_dict.vector == vector(1.1,1.1,1.1)
    assert test_dict.tensor == tensor(1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1)
    assert test_dict.wordList.list() == ["word1", "word2"]
    assert (test_dict.scalarField == np.ones(2)).all()
    assert (test_dict.vectorField == np.ones([2,3])).all()
    assert (test_dict.tensorField == np.ones([2,9])).all()
    assert test_dict.subDict.word2 == "word2"