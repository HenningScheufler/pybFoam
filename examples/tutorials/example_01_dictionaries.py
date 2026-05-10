"""
Read, modify, and write OpenFOAM dictionaries
=============================================

This tutorial covers how to:

- read an OpenFOAM dictionary file from disk into a Python object,
- pull typed values out of it (``Word``, ``float``, sub-dictionaries),
- change a value, and
- write the dictionary back to disk in OpenFOAM format.

Prerequisites
-------------

- OpenFOAM v2312+ sourced (``source /path/to/OpenFOAM/etc/bashrc``).
- pybFoam installed (``pip install pybFoam[all]``).
"""

# %%
# Set up a working case
# ---------------------
# :func:`pybFoam.clone_example` copies a baseline case from the repo's
# ``examples/`` folder into a tmp directory and restores ``0.orig/``
# → ``0/``. The original on-disk case is never touched.

from pybFoam import clone_example

case = clone_example("cavity")
print(f"case = {case}")

# %%
# Read a dictionary
# -----------------
# :func:`pybFoam.dictionary.read` parses an OpenFOAM dictionary into a
# Python object. ``get[T]`` is templated on the expected return type —
# the same accessor handles scalars, words, vectors, and field
# entries; a wrong type raises immediately at the call site.

from pybFoam import Word, dictionary

control_dict_path = case / "system" / "controlDict"
d = dictionary.read(str(control_dict_path))

print("application :", d.get[Word]("application"))
print("startTime   :", d.get[float]("startTime"))
print("endTime     :", d.get[float]("endTime"))
print("deltaT      :", d.get[float]("deltaT"))

# %%
# Enumerate the top-level keys with ``dictionary.toc``. Use this
# to introspect any dictionary you didn't write yourself.

print("top-level keys:")
for key in d.toc():
    print(f"  - {key}")

# %%
# Modify and write it back
# ------------------------
# ``dictionary.set`` updates an entry in place. ``dictionary.write``
# serialises the whole dictionary back to disk in OpenFOAM format. We
# write to a sibling path so we can compare before/after.

modified_path = case / "system" / "controlDict.modified"

d.set("endTime", 0.05)
d.set("deltaT", 0.005)
d.write(str(modified_path))

# %%
# The round-trip via disk proves the values were actually written —
# not just held in the in-memory dictionary object.

d_modified = dictionary.read(str(modified_path))
print("endTime (modified) :", d_modified.get[float]("endTime"))
print("deltaT  (modified) :", d_modified.get[float]("deltaT"))

# %%
# Skim the output file directly to see what an OpenFOAM dictionary
# looks like on disk (``set`` preserves OpenFOAM's syntax).

for line in modified_path.read_text().splitlines()[-20:]:
    print(line)
