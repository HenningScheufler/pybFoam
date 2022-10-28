#!/bin/sh
cd "${0%/*}" || exit                                # Run from this directory

git submodule update --init --recursive
pip install -r requirements.txt

#install generator
cd OpenFOAMGen/OpenFOAMGen
conan export . myuser/OpenFOAMGen
cd ../..
# compile pybFoam
./Allwmake

pip install . -v

