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


def test_ofdict(change_test_dir):

    d = dictionary.read("system/TestDict")

    l_word = d.toc()
    assert l_word.list()[0] == "FoamFile"

    # primitives

    
    assert d.get_word("word") == "word"
    assert d.get_scalar("scalar") == 1.1
    assert d.get_vector("vector") == vector(1.1,1.1,1.1)
    assert d.get_tensor("tensor") == tensor(1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1)

    # fields
    assert  d.get_wordList("wordList").list() == ["word1", "word2"]
    assert (d.get_scalarField("scalarField") == np.ones(2)).all()
    assert (d.get_vectorField("vectorField") == np.ones([2,3])).all()
    assert (d.get_tensorField("tensorField") == np.ones([2,9])).all()

    # assumes the other are also correct
    d.set("word",Word("word2"))

    assert d.get_word("word") == "word2"

    d.set("wordList2",pybFoam.wordList(["word3","word4"]))

    assert  d.get_wordList("wordList2").list() == ["word3", "word4"]

    subDict = d.subDict("subDict")
    assert subDict.get_word("word2") == "word2"
