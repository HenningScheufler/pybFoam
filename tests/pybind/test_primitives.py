import numpy as np

from pybFoam import Word, mag, scalarField, tensor, tensorField, vector, vectorField


def test_mag() -> None:
    s = -10
    assert mag(s) == 10


def test_word() -> None:
    w = Word("test")
    assert w == "test"


def test_vector() -> None:
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


def test_tensor() -> None:
    # test add und comparision
    ten = tensor(0, 0, 0, 0, 0, 1, 1, 1, 1) + tensor(1, 1, 1, 1, 1, 0, 0, 0, 0)
    assert ten == (1, 1, 1, 1, 1, 1, 1, 1, 1)

    # test subtract
    ten2 = ten - tensor(1, 1, 1, 1, 1, 0, 0, 0, 0)
    assert ten2 == (0, 0, 0, 0, 0, 1, 1, 1, 1)

    # test multiply
    ten2 = ten2 * 2
    assert ten2 == tensor(0, 0, 0, 0, 0, 2, 2, 2, 2)

    assert tensor(1, 0, 0, 0, 1, 0, 0, 0, 1) & vector(1, 2, 3) == (1, 2, 3)

    # test comparisions
    assert ten2 != ten
    assert mag(ten) == 3


def test_scalar_field_buffer() -> None:
    f = scalarField([0] * 10)

    a = np.asarray(f)
    assert a.shape == (10,)
    assert a.dtype == np.float64
    assert np.allclose(a, 0.0)  # assuming default constructor zeros it
    a += 10
    assert np.allclose(a, 10.0)
    assert f[0] == 10.0


def test_vector_field_buffer() -> None:
    f = vectorField([vector(0, 0, 0) for _ in range(0, 4)])
    a = np.asarray(f)
    assert a.shape == (4, 3)
    assert a.dtype == np.float64
    assert np.allclose(a, 0.0)
    a += [10, 10, 10]
    assert np.allclose(a, 10.0)
    assert f[0][0] == 10.0


def test_tensor_field_buffer() -> None:
    f = tensorField([tensor(0, 0, 0, 0, 0, 0, 0, 0, 0) for _ in range(0, 2)])
    a = np.asarray(f)
    assert a.shape == (2, 9)
    assert a.dtype == np.float64
    assert np.allclose(a, 0.0)
    a += [10, 10, 10, 10, 10, 10, 10, 10, 10]
    assert np.allclose(a, 10.0)
    assert f[0][0] == 10.0


def test_scalarField() -> None:
    sf = scalarField()
    assert len(sf) == 0

    sf2 = scalarField([1, 2, 3, 4, 5, 6])

    assert len(sf2) == 6
    assert sf2[3] == 4
    sf2[3] = 0
    assert sf2[3] == 0

    sf3 = scalarField([1, 2, 3, 4, 5, 6]) * 3
    assert sf3[0] == 3

    sf_1 = scalarField([1 for i in range(0, 6)])
    assert (sf_1 == np.ones(6)).all()

    sf_1 += 10
    assert sf_1[0] == 11

    for scalar in sf_1:
        assert scalar == 11


def test_vectorField() -> None:
    vf = vectorField()
    assert len(vf) == 0

    vf = vectorField([[0, 0, 0], [1, 1, 1]])
    assert len(vf) == 2
    assert vf[0] == vector(0, 0, 0)
    assert vf[1] == vector(1, 1, 1)

    vf2 = vectorField([vector(i, i, i) for i in range(1, 7)])
    assert len(vf2) == 6
    assert vf2[1][1] == 2

    vf3 = vectorField([vector(i, i, i) for i in range(0, 6)]) * 3
    assert vf3[1][1] == 3

    vf4 = vf2 + vf3
    assert vf4[1][1] == 5

    vf4 = vf2 - vf3
    assert vf4[1][1] == -1

    sf1 = scalarField([i for i in range(0, 6)])
    vf4 = vf2 * sf1
    assert vf4[1][1] == 2

    vf_1 = vectorField([vector(1, 1, 1) for i in range(0, 6)])
    assert (vf_1 == np.ones([6, 3])).all()

    # scalar product
    vf_5 = vectorField([vector(1, 2, 3) for i in range(0, 6)])
    vf_6 = vectorField([vector(1, 1, 1) for i in range(0, 6)])

    assert (np.asarray(vf_6 & vf_5) == np.array([6, 6, 6, 6, 6, 6])).all()

    # assert (np.asarray(vector(1,1,1) & vf_5) == np.array([6,6,6,6,6,6])).all() # not supported
    assert (np.asarray(vf_5 & vector(1, 1, 1)) == np.array([6, 6, 6, 6, 6, 6])).all()


def test_tensorField() -> None:
    tf = tensorField()
    assert len(tf) == 0

    tf = tensorField([[0, 0, 0, 0, 0, 0, 0, 0, 0], [1, 1, 1, 1, 1, 1, 1, 1, 1]])
    assert len(tf) == 2
    assert tf[0] == tensor(0, 0, 0, 0, 0, 0, 0, 0, 0)
    assert tf[1] == tensor(1, 1, 1, 1, 1, 1, 1, 1, 1)

    tf2 = tensorField([tensor(i, i, i, i, i, i, i, i, i) for i in range(1, 7)])
    assert len(tf2) == 6
    assert tf2[1][1] == 2

    tf3 = tensorField([tensor(i, i, i, i, i, i, i, i, i) for i in range(0, 6)]) * 3
    assert tf3[1][1] == 3

    tf4 = tf2 + tf3
    assert tf4[1][1] == 5

    tf4 = tf2 - tf3
    assert tf4[1][1] == -1

    sf1 = scalarField([i for i in range(0, 6)])
    tf4 = tf2 * sf1
    assert tf4[1][1] == 2

    tf_1 = tensorField([tensor(1, 1, 1, 1, 1, 1, 1, 1, 1) for i in range(0, 6)])
    assert (tf_1 == np.ones([6, 9])).all()


def test_tmp_scalarField() -> None:
    """Test tmp<scalarField> type availability."""
    from pybFoam import tmp_scalarField

    # Verify the type is available
    assert tmp_scalarField is not None


def test_tmp_scalarField_dereference() -> None:
    """Test tmp<scalarField> dereference operation.

    Note: In practice, tmp fields are returned from C++ operations.
    The __call__ operator converts tmp to Field.
    """
    from pybFoam import tmp_scalarField

    # Verify the type exists and documents expected usage:
    # tmp_result = some_cpp_function()  # Returns tmp<scalarField>
    # field = tmp_result()              # Dereference to scalarField
    assert tmp_scalarField is not None


def test_tmp_scalarField_operators() -> None:
    """Test tmp<scalarField> arithmetic operators.

    Documents the operator support for tmp<scalarField>:
    - __call__: Dereference to scalarField
    - __neg__: Unary negation
    - __add__, __sub__: Addition and subtraction with Field, tmp<Field>, and scalars
    - __mul__, __rmul__: Multiplication with scalars and fields
    - __truediv__: Division by scalars and fields
    - __len__, __getitem__: Length and indexing
    """
    from pybFoam import tmp_scalarField

    # Verify operator methods exist
    assert hasattr(tmp_scalarField, "__call__")
    assert hasattr(tmp_scalarField, "__neg__")
    assert hasattr(tmp_scalarField, "__add__")
    assert hasattr(tmp_scalarField, "__sub__")
    assert hasattr(tmp_scalarField, "__mul__")
    assert hasattr(tmp_scalarField, "__truediv__")
    assert hasattr(tmp_scalarField, "__len__")
    assert hasattr(tmp_scalarField, "__getitem__")


def test_tmp_vectorField() -> None:
    """Test tmp<vectorField> operations including dot products."""
    from pybFoam import tmp_vectorField

    # Test that the type is available
    assert tmp_vectorField is not None

    # The tmp_vectorField has additional __and__ operators for dot products:
    # - tmp & vector -> tmp<scalarField>
    # - tmp & Field<vector> -> tmp<scalarField>
    # - tmp & tmp<Field<vector>> -> tmp<scalarField>
    assert hasattr(tmp_vectorField, "__and__")


def test_tmp_tensorField() -> None:
    """Test tmp<tensorField> and tmp<symmTensorField> availability."""
    from pybFoam import tmp_symmTensorField, tmp_tensorField

    assert tmp_tensorField is not None
    assert tmp_symmTensorField is not None


def test_field_tmp_field_operations() -> None:
    """Test arithmetic operations between Field and tmp<Field>."""
    from pybFoam import scalarField

    # Create test fields
    a = scalarField([1.0, 2.0, 3.0, 4.0, 5.0])
    b = scalarField([2.0, 3.0, 4.0, 5.0, 6.0])
    c = scalarField([3.0, 4.0, 5.0, 6.0, 7.0])

    # Test Field + tmp<Field> (tmp result from multiplication)
    tmp_result = a * 2.0  # Returns tmp<scalarField>
    result = b + tmp_result
    assert len(result) == 5

    # Test Field - tmp<Field>
    result = c - tmp_result
    assert len(result) == 5

    # Test Field * tmp<Field>
    result = a * tmp_result
    assert len(result) == 5

    # Test Field / tmp<Field>
    result = b / tmp_result
    assert len(result) == 5

    # Test complex expression: Field - Field * Field (reproduces benchmark issue)
    x = a * b  # Returns tmp<scalarField>
    y = c - x  # Field - tmp<Field>
    assert len(y) == 5

    # Even more complex: (Field * Field + Field) / (Field + scalar)
    x = a * b + c  # tmp + Field -> tmp
    y = c - a * b  # Field - tmp
    result = (x + y) / (a + 1.0)  # All mixed operations
    assert len(result) == 5


def test_tmp_field_mixed_operations() -> None:
    """Test tmp<Field> operations with various operand types."""
    from pybFoam import scalarField

    a = scalarField([2.0, 4.0, 6.0, 8.0])
    b = scalarField([1.0, 2.0, 3.0, 4.0])

    # tmp + Field
    tmp_a = a * 2.0
    result1 = tmp_a + b
    assert len(result1) == 4

    # tmp + tmp
    tmp_b = b * 3.0
    result2 = tmp_a + tmp_b
    assert len(result2) == 4

    # tmp + scalar
    result3 = tmp_a + 5.0
    assert len(result3) == 4

    # tmp - Field
    result4 = tmp_a - b
    assert len(result4) == 4

    # tmp - tmp
    result5 = tmp_a - tmp_b
    assert len(result5) == 4

    # tmp * Field
    result6 = tmp_a * b
    assert len(result6) == 4

    # tmp / Field
    result7 = tmp_a / b
    assert len(result7) == 4
