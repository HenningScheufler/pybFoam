import pytest
from pybFoam import dictionary, vector, tensor
import os
import numpy as np

@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)


def test_ofdict(change_test_dir):

    d = dictionary("system/TestDict")

    l_word = d.toc()
    assert l_word.list()[0] == "FoamFile"

    # primitives
    assert d.get_word("word") == "word"
    assert d.get_scalar("scalar") == 1.1
    assert d.get_vector("vector") == vector(1.1,1.1,1.1)
    assert d.get_tensor("tensor") == tensor(1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1,1.1)

    # fields
    assert  d.get_wordList("wordList").list() == ["word1", "word2"]
    assert (d.get_scalarField("scalarField").to_numpy() == np.ones(2)).all()
    assert (d.get_vectorField("vectorField").to_numpy() == np.ones([2,3])).all()
    assert (d.get_tensorField("tensorField").to_numpy() == np.ones([2,9])).all()
