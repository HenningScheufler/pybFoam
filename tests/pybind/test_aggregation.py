import pytest
from pybFoam import vector, boolList, labelList, mag, scalarField, vectorField, tensorField
from pybFoam import aggregation
import numpy as np


def test_mag():
    s = -10
    assert mag(s) == 10


def test_sum():
    field = scalarField([1, 2, 3])
    assert aggregation.sum(field, boolList([True, True, True]), None).values[0] == 6
    assert aggregation.sum(field, None, None).values[0] == 6
    assert aggregation.sum(field, boolList([True, False, True]), None).values[0] == 4

    field = vectorField([vector(1, 2, 3), vector(4, 5, 6)])
    assert aggregation.sum(field, None, None).values[0] == vector(5, 7, 9)
    assert aggregation.sum(field, boolList([True, True]), None).values[0] == vector(
        5, 7, 9
    )
    assert aggregation.sum(field, boolList([True, False]), None).values[0] == vector(
        1, 2, 3
    )

    # # with groupby
    field = scalarField([1, 2, 3])
    assert aggregation.sum(field, None, labelList([0, 1, 1])).values[0] == 1
    assert aggregation.sum(field, None, labelList([0, 1, 1])).values[1] == 5

    assert aggregation.sum(field, boolList([True, False, True]), labelList([0, 1, 1])).values[0] == 1
    assert aggregation.sum(field, boolList([True, False, True]), labelList([0, 1, 1])).values[1] == 3


def test_min():

    field = scalarField([1, 2, 3])
    assert aggregation.min(field, None, None).values[0] == 1
    assert aggregation.min(field, boolList([True, True, True]), None).values[0] == 1
    assert aggregation.min(field, boolList([True, False, True]), None).values[0] == 1

    field = vectorField([vector(1, 2, 3), vector(4, 5, 6)])
    assert aggregation.min(field, None, None).values[0] == vector(1, 2, 3)
    assert aggregation.min(field, boolList([True, True]), None).values[0] == vector(
        1, 2, 3
    )
    assert aggregation.min(field, boolList([True, False]), None).values[0] == vector(
        1, 2, 3
    )

    # # with groupby
    field = scalarField([1, 2, 3])
    assert aggregation.min(field, None, labelList([0, 1, 1])).values[0] == 1
    assert aggregation.min(field, None, labelList([0, 1, 1])).values[1] == 2

    assert aggregation.min(field, boolList([True, False, True]), labelList([0, 1, 1])).values[0] == 1
    assert aggregation.min(field, boolList([True, False, True]), labelList([0, 1, 1])).values[1] == 3

def test_max():

    field = scalarField([1, 2, 3])
    assert aggregation.max(field, None, None).values[0] == 3
    assert aggregation.max(field, boolList([True, True, True]), None).values[0] == 3
    assert aggregation.max(field, boolList([True, False, True]), None).values[0] == 3

    field = vectorField([vector(1, 2, 3), vector(4, 5, 6)])
    assert aggregation.max(field, None, None).values[0] == vector(4, 5, 6)
    assert aggregation.max(field, boolList([True, True]), None).values[0] == vector(
        4, 5, 6
    )
    assert aggregation.max(field, boolList([True, False]), None).values[0] == vector(
        1, 2, 3
    )

    # # with groupby
    field = scalarField([1, 2, 3])
    assert aggregation.max(field, None, labelList([0, 1, 1])).values[0] == 1
    assert aggregation.max(field, None, labelList([0, 1, 1])).values[1] == 3

    assert aggregation.max(field, boolList([True, False, True]), labelList([0, 1, 1])).values[0] == 1
    assert aggregation.max(field, boolList([True, False, True]), labelList([0, 1, 1])).values[1] == 3