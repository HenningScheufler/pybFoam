from __future__ import annotations

from typing import Any, Dict, List, Sequence

from pybFoam.pybFoam_core import Word, dictionary, labelList, scalarField, vector, wordList


def _ensure_len(v: Sequence[float], n: int, name: str) -> List[float]:
    """Validate that a sequence has exactly n elements and convert to float list."""
    if not isinstance(v, (list, tuple)):
        raise TypeError(f"{name} must be a list/tuple of length {n}")
    if len(v) != n:
        raise ValueError(f"{name} must have length {n}")
    return [float(x) for x in v]


def dict_to_foam(py_dict: Dict[str, Any]) -> dictionary:
    """Convert a Python dictionary to an OpenFOAM dictionary object.

    This helper converts nested Python dicts and lists into OpenFOAM
    dictionary objects that can be passed to C++ bindings.

    Args:
        py_dict: Python dictionary with string keys and various value types

    Returns:
        OpenFOAM dictionary object
    """

    foam_dict = dictionary()

    for key, value in py_dict.items():
        if isinstance(value, str):
            foam_dict.set(key, Word(value))
        elif isinstance(value, bool):
            foam_dict.set(key, value)
        elif isinstance(value, (int, float)):
            foam_dict.set(key, value)
        elif isinstance(value, list):
            # Check if it's a vector (3 numbers) or a list of strings
            if len(value) == 3 and all(isinstance(x, (int, float)) for x in value):
                foam_dict.set(key, vector(float(value[0]), float(value[1]), float(value[2])))
            elif all(isinstance(x, str) for x in value):  # wordList
                # List of strings -> wordList
                wlist = wordList(value)
                foam_dict.set(key, wlist)
            elif all(isinstance(x, float) for x in value):
                # List of numbers
                sField = scalarField(value)
                foam_dict.set(key, sField)
            elif all(isinstance(x, int) for x in value):
                # List of numbers
                lList = labelList([int(x) for x in value])
                foam_dict.set(key, lList)
            else:
                # Mixed or nested - try as-is
                foam_dict.set(key, value)  # type: ignore[call-overload]
        elif isinstance(value, dict):
            # Nested dictionary
            foam_dict.set(key, dict_to_foam(value))
        else:
            # Try setting as-is
            foam_dict.set(key, value)

    return foam_dict


__all__ = ["_ensure_len", "dict_to_foam"]
