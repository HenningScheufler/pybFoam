# pybFoam

python bindings for OpenFOAM

currently in the pre-alpha release state


## Installation

requires OpenFOAM of2012 or higher sourced and installed and python 3.7+ (conda is highly recommended) 

```
./build-pybFoam.sh # will install conan zmq oftest
```
## Testsuite

oftest is automatically installed and is run with py.test

```
py.test
```

## Build Documentation (WIP)

The documentation is based on the [Jekyll Documentation Theme](https://idratherbewriting.com/documentation-theme-jekyll/)

```
cd docs
jekyll serve
```