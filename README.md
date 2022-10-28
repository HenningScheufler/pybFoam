# ECI4I4FOAM

External Coupling Interface 4 FOAM ECI4FOAM provides an interface for coupling external tools and software to OpenFOAM

# Documentation

[Documentation](https://DLR-RY.github.io/ECI4FOAM/)

## Installation

requires OpenFOAM of2012 or higher sourced and installed and python 3.7+ (conda is highly recommended) 

```
./build-ECI4FOAM.sh # will install conan zmq oftest
```
## Testsuite

install oftest to automatically test OpenFOAM with py.test

```
pip install oftest
```

run the test enviroment
```
py.test
```

## Build Documentation

The documentation is based on the [Jekyll Documentation Theme](https://idratherbewriting.com/documentation-theme-jekyll/)

```
cd docs
jekyll serve
```