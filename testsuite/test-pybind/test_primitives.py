import pytest
from pybFoam import vector, Word, tensor, mag, scalarField, vectorField, tensorField
import numpy as np

def test_word():
    s = -10
    assert mag(s) == 10


def test_word():
    w = Word("test")
    assert w == "test"


def test_vector():
    # test add und comparision
    vec = vector(0, 0, 4) + vector(0, 3, 0)
    assert vec == (0, 3, 4)

    # test init from Tuple
    vecTuple = vector((0, 3, 4))
    assert vec == vecTuple

    # test subtract
    vec2 = vec - vector(0, 3, 0)
    assert vec2 == (0, 0, 4)

    # test multiply
    vec2 = vec2 * 2
    assert vec2 == vector(0, 0, 8)
    assert vec2 & vector(0, 4, 0) == 0

    # test comparisions
    assert vec2 != vec
    assert mag(vec) == 5


def test_tensor():
    # test add und comparision
    ten = tensor(0, 0, 0, 0, 0, 1, 1, 1, 1) + tensor(1, 1, 1, 1, 1, 0, 0, 0, 0)
    assert ten == (1, 1, 1, 1, 1, 1, 1, 1, 1)

    # test subtract
    ten2 = ten - tensor(1, 1, 1, 1, 1, 0, 0, 0, 0)
    assert ten2 == (0, 0, 0, 0, 0, 1, 1, 1, 1)

    # test multiply
    ten2 = ten2 * 2
    assert ten2 == tensor(0, 0, 0, 0, 0, 2, 2, 2, 2)

    assert tensor(1, 0, 0,
                  0, 1, 0,
                  0, 0, 1) & vector(1, 2, 3) == (1,2,3)

    # test comparisions
    assert ten2 != ten
    assert mag(ten) == 3


def test_scalarField():

    sf = scalarField()
    assert len(sf) == 0
    
    sf2 = scalarField([1,2,3,4,5,6])

    assert len(sf2) == 6
    assert sf2[3] == 4
    sf2[3] = 0
    assert sf2[3] == 0

    sf3 = scalarField([1,2,3,4,5,6])*3
    assert sf3[0] == 3

    sf_1 = scalarField([1 for i in range(0,6)])
    assert (sf_1.to_numpy() == np.ones(6)).all()

def test_vectorField():

    vf = vectorField()
    assert len(vf) == 0
    
    vf2 = vectorField([vector(i,i,i) for i in range(1,7)])
    assert len(vf2) == 6
    assert vf2[1][1] == 2
    
    vf3 = vectorField([vector(i,i,i) for i in range(0,6)])*3
    assert vf3[1][1] == 3

    vf4 = vf2 + vf3
    assert vf4[1][1] == 5

    vf4 = vf2 - vf3
    assert vf4[1][1] == -1

    sf1 = scalarField([i for i in range(0,6)])
    vf4 = vf2 * sf1
    assert vf4[1][1] == 2

    vf_1 = vectorField([vector(1,1,1) for i in range(0,6)])
    assert (vf_1.to_numpy() == np.ones([6,3])).all()


def test_tensorField():

    tf = tensorField()
    assert len(tf) == 0
    
    tf2 = tensorField([tensor(i,i,i,i,i,i,i,i,i) for i in range(1,7)])
    assert len(tf2) == 6
    assert tf2[1][1] == 2
    
    tf3 = tensorField([tensor(i,i,i,i,i,i,i,i,i) for i in range(0,6)])*3
    assert tf3[1][1] == 3

    tf4 = tf2 + tf3
    assert tf4[1][1] == 5

    tf4 = tf2 - tf3
    assert tf4[1][1] == -1

    sf1 = scalarField([i for i in range(0,6)])
    tf4 = tf2 * sf1
    assert tf4[1][1] == 2

    tf_1 = tensorField([tensor(1,1,1,1,1,1,1,1,1) for i in range(0,6)])
    assert (tf_1.to_numpy() == np.ones([6,9])).all()