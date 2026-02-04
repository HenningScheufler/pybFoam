"""
Test interoperability between Field and tmp<Field> types.

This test suite verifies that arithmetic operations work correctly
when mixing Field and tmp<Field> operands, which is essential for
writing natural expressions like:

    result = field1 - field2 * field3

Where field2 * field3 returns a tmp<Field>.
"""

from pybFoam import scalarField, vectorField


def test_field_plus_tmp() -> None:
    """Test Field + tmp<Field>."""
    a = scalarField([1.0, 2.0, 3.0])
    b = scalarField([2.0, 3.0, 4.0])

    # b * 2.0 returns tmp<scalarField>
    result = a + (b * 2.0)
    assert len(result) == 3


def test_field_minus_tmp() -> None:
    """Test Field - tmp<Field> (was failing before fix)."""
    a = scalarField([5.0, 6.0, 7.0])
    b = scalarField([2.0, 3.0, 4.0])

    # This was the failing case from benchmark
    result = a - (b * 2.0)
    assert len(result) == 3


def test_field_times_tmp() -> None:
    """Test Field * tmp<Field>."""
    a = scalarField([2.0, 3.0, 4.0])
    b = scalarField([1.0, 2.0, 3.0])

    result = a * (b + 1.0)
    assert len(result) == 3


def test_field_div_tmp() -> None:
    """Test Field / tmp<Field>."""
    a = scalarField([10.0, 20.0, 30.0])
    b = scalarField([2.0, 4.0, 5.0])

    result = a / (b * 1.0)
    assert len(result) == 3


def test_tmp_plus_field() -> None:
    """Test tmp<Field> + Field."""
    a = scalarField([1.0, 2.0, 3.0])
    b = scalarField([2.0, 3.0, 4.0])

    result = (a * 2.0) + b
    assert len(result) == 3


def test_tmp_minus_field() -> None:
    """Test tmp<Field> - Field."""
    a = scalarField([5.0, 6.0, 7.0])
    b = scalarField([2.0, 3.0, 4.0])

    result = (a * 2.0) - b
    assert len(result) == 3


def test_tmp_times_field() -> None:
    """Test tmp<Field> * Field."""
    a = scalarField([2.0, 3.0, 4.0])
    b = scalarField([1.0, 2.0, 3.0])

    result = (a + 1.0) * b
    assert len(result) == 3


def test_tmp_div_field() -> None:
    """Test tmp<Field> / Field."""
    a = scalarField([10.0, 20.0, 30.0])
    b = scalarField([2.0, 4.0, 5.0])

    result = (a * 1.0) / b
    assert len(result) == 3


def test_tmp_plus_tmp() -> None:
    """Test tmp<Field> + tmp<Field>."""
    a = scalarField([1.0, 2.0, 3.0])
    b = scalarField([2.0, 3.0, 4.0])

    result = (a * 2.0) + (b * 3.0)
    assert len(result) == 3


def test_tmp_minus_tmp() -> None:
    """Test tmp<Field> - tmp<Field>."""
    a = scalarField([5.0, 6.0, 7.0])
    b = scalarField([2.0, 3.0, 4.0])

    result = (a * 2.0) - (b * 3.0)
    assert len(result) == 3


def test_complex_expression_benchmark() -> None:
    """Test the exact expression from the complex benchmark that was failing."""
    a = scalarField([1.1, 2.1, 3.1])
    b = scalarField([2.2, 3.2, 4.2])
    c = scalarField([3.3, 4.3, 5.3])
    d = scalarField([4.4, 5.4, 6.4])

    # This is the exact expression from benchmark_expression_complex.py
    x = a * b + c
    y = d - a * c  # This line was failing: Field - tmp
    result = (x * y + b) / (a + 1.0)

    assert len(result) == 3


def test_chained_tmp_operations() -> None:
    """Test multiple chained operations that return tmp."""
    a = scalarField([1.0, 2.0, 3.0, 4.0, 5.0])
    b = scalarField([2.0, 3.0, 4.0, 5.0, 6.0])
    c = scalarField([3.0, 4.0, 5.0, 6.0, 7.0])

    # All these operations return tmp, but should chain correctly
    result = ((a + b) * c - a) / (b + 1.0)
    assert len(result) == 5


def test_field_scalar_operations_with_tmp() -> None:
    """Test operations between Field<scalar> and tmp<Field<scalar>>."""
    a = scalarField([1.0, 2.0, 3.0])

    # Field operations with tmp results
    result1 = a * (a + 1.0)  # Field * tmp
    result2 = (a * 2.0) / a  # tmp / Field
    result3 = a + (a * 2.0) - (a / 2.0)  # Mixed

    assert len(result1) == 3
    assert len(result2) == 3
    assert len(result3) == 3


def test_vectorfield_tmp_operations() -> None:
    """Test tmp operations with vectorField."""
    vf = vectorField([[1, 2, 3], [4, 5, 6]])

    # Vector operations that return tmp
    result = vf * 2.0
    assert len(result) == 2

    # Field + tmp
    result2 = vf + (vf * 0.5)
    assert len(result2) == 2


def test_mixed_type_scalar_field() -> None:
    """Test that tmp<Field<scalar>> can be used with regular scalar operations."""
    a = scalarField([10.0, 20.0, 30.0])

    # Scalar operations
    tmp1 = a * 2.0
    tmp2 = a / 2.0
    tmp3 = a + 5.0
    tmp4 = a - 3.0

    # All should return tmp and work in expressions
    result = tmp1 + tmp2 - tmp3 + tmp4
    assert len(result) == 3


def test_operation_return_types() -> None:
    """Document which operations return tmp vs Field."""
    from pybFoam import tmp_scalarField

    a = scalarField([1.0, 2.0, 3.0])
    b = scalarField([4.0, 5.0, 6.0])

    # Operations that return tmp<Field>
    tmp_result = a * 2.0
    assert isinstance(tmp_result, tmp_scalarField)

    tmp_result = a + b
    assert isinstance(tmp_result, tmp_scalarField)

    tmp_result = a - (b * 2.0)
    assert isinstance(tmp_result, tmp_scalarField)

    # Can dereference tmp to Field using ()
    field_result = tmp_result()
    assert isinstance(field_result, scalarField)
    assert not isinstance(field_result, tmp_scalarField)
