# Conan for OpenFOAM

Simplifies the linking process of external libraries to OpenFOAM

New libraries can be download easily from the conancenter and linked to OpenFOAM


# Prerequisite

Assuming we already have anaconda and OpenFOAM is sourced installed:


```bash
    conda create -n conan python=3.9 # we dont want break our install
    conda activate conan
    pip install conan # conan is required
```



# Usage


cat option.template:
```
CONAN_INCS := \
    {{{CONAN_INCS_INCS}}}

CONAN_LIBS := \
    {{{CONAN_INCS_LIBS}}}


EXE_INC = \
    -I../../../src/lnInclude \
    -I$(LIB_SRC)/finiteVolume/lnInclude \
    -I$(LIB_SRC)/meshTools/lnInclude \
    $(CONAN_INCS)

EXE_LIBS = \
    -lfiniteVolume \
    -lmeshTools \
    $(CONAN_LIBS)
```

add CONAN_INCS and CONAN_LIBS to your options file and place it in the Make folder.
The generator will replace 

{{{CONAN_INCS_INCS}}}

and 

{{{CONAN_INCS_LIBS}}}

with the cached libraries in conan


## Install OpenFOAM generator

```bash
    cd OpenFOAMGen
    conan export . myuser/OpenFOAMGen
```

The generator is now installed


## Compile OpenFOAM with Boost regexp and lohmann json



```bash
    cd test-conan
    conan install . -s compiler=gcc -s compiler.libcxx=libstdc++11 --build
```

boost and the json library are now downloaded and compiled. Conan needs to know the compiler version and the c++ standard to download the correct version. 

Note: this will take a while.


```bash
    wmake # compiles est-boostReg-CreditCardNumber and links it with conan-libs
    test-boostReg-CreditCardNumber 1000-1000-1000-1000 
```

And you should see:
```
dumping json{
    "answer": {
        "everything": 42
    },
    "happy": true,
    "list": [
        1,
        0,
        2
    ],
    "name": "Niels",
    "nothing": null,
    "object": {
        "currency": "USD",
        "value": 42.99
    },
    "pi": 3.141
}
boost regexp
Matched OK...
```
