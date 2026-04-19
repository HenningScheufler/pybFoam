"""
Read an OpenFOAM dictionary
===========================

Use :func:`pybFoam.dictionary.read` to load a case dictionary from disk
and pull typed values out of it. The same accessor — ``get[T]`` — works
for scalars, words, vectors, field entries, and nested subdicts.

We exercise the API against the shipped ``examples/case/system/`` files:
``controlDict`` (well-known OpenFOAM entries) and ``TestDict`` (a
purpose-built dictionary demonstrating every typed entry kind).
"""

# %%
# Locate the case
# ---------------
# We walk up from the script's working directory (set by sphinx-gallery
# to the script's own folder) until we find the shared ``examples/``
# root, then reach the target case from there.

from pathlib import Path


def _examples_root() -> Path:
    for p in [Path.cwd(), *Path.cwd().parents]:
        if p.name == "examples":
            return p
    raise RuntimeError("Could not locate examples/ root")


CASE = _examples_root() / "case"

# %%
# Read controlDict and pull typed values
# --------------------------------------
# ``get[T]`` is templated on the expected return type. A typo in the key
# raises at the point of use.

from pybFoam import Word, dictionary

d = dictionary.read(str(CASE / "system" / "controlDict"))

application = d.get[Word]("application")
end_time = d.get[float]("endTime")
delta_t = d.get[float]("deltaT")
max_co = d.getOrDefault[float]("maxCo", 0.5)

print(f"application = {application}")
print(f"endTime     = {end_time}")
print(f"deltaT      = {delta_t}")
print(f"maxCo       = {max_co}")

# %%
# Enumerate top-level keys
# ------------------------

print("top-level keys:")
for name in d.toc():
    print(f"  {name}")

# %%
# Descend into a subdictionary
# ----------------------------
# ``subDict`` returns another ``dictionary`` that keeps working with the
# same accessors.

funcs = d.subDict("functions")
print("function objects:", list(funcs.toc()))

# %%
# Typed field entries
# -------------------
# ``scalarField``, ``vectorField``, ``tensorField``, and ``wordList``
# entries are returned by the same ``get[T]()`` interface and can be
# viewed as NumPy arrays with no copy.

import numpy as np

import pybFoam

td = dictionary.read(str(CASE / "system" / "TestDict"))

scalars = td.get[pybFoam.scalarField]("scalarField")
vectors = td.get[pybFoam.vectorField]("vectorField")
words = td.get[pybFoam.wordList]("wordList").list()

print("scalarField:", np.asarray(scalars))
print("vectorField shape:", np.asarray(vectors).shape)
print("wordList:", words)

# %%
# Single typed values (word/scalar/vector/tensor) are accessed the same way:

print("word   :", td.get[Word]("word"))
print("scalar :", td.get[float]("scalar"))
print("vector :", np.asarray(td.get[pybFoam.vector]("vector")))
print("tensor :", np.asarray(td.get[pybFoam.tensor]("tensor")))
