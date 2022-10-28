#!/bin/bash


function pyver {
    PY_VNUMBER="$(python --version | cut -d " " -f 2)"
    PY_MAJOR=$(echo $PY_VNUMBER  | cut -d "." -f 1)
    PY_MINOR=$(echo $PY_VNUMBER | cut -d "." -f 2)
    PYVERSION="$PY_MAJOR.$PY_MINOR"
    echo -e $PYVERSION
}

function pylib {
    py_version=$(pyver)
    PYVERSION="-lpython$py_version"
    echo -e $PYVERSION
}

function pyincludes {
    PYINC=$(python3-config --includes | sed 's/ / \\ \\n/g')
    # echo $($PY_INCS | sed 's/ / \\ \\n/g')
    includes="PY_INCS := \ \n
        -Wno-old-style-cast \ \n""$PYINC"
        
    echo -e $includes
}

function pyincludeswithNumpy {
    PYINC=$(python3-config --includes | sed 's/ / \\ \\n/g')
    py_version=$(pyver)
    PYPREFIX=$(python3-config --prefix)
    includes="PY_INCS := \ \n
        -Wno-old-style-cast \ \n""$PYINC"
    includes="$includes \ \n-I$PYPREFIX/lib/python$py_version/site-packages/numpy/core/include" 
      
    echo -e $includes
}

function pybind11_includes {
    PYINC=$(python3 -m pybind11 --includes) 
    includes="PY_INCS := \ \n
        -Wno-old-style-cast \ \n""$PYINC"
      
    echo -e $includes
}

function copyPythonIncs {
    incs=$(python3-config --includes)
    for inc in $incs
    do
        inc="${inc:2}"
        echo "${inc}"
        cp -r "${inc}/." $1
    done
}

function pylibs {
    flags=$(python3-config --ldflags)
    PYFLAGS="PY_LIBS := \ \n"
    for f in $flags
    do
        if [[ $f == *"-L"* ]]; then
            PYFLAGS+="$f \ \n"
        fi
    done
    if [[ $1 = "-rpath" ]]; then
        rpath=$(pyrpath)
        PYFLAGS+="$rpath \ \n"
    fi
    PYFLAGS+=$(pylib)
    echo -e $PYFLAGS
}

function pyrpath {
    PYPREFIX=$(python3-config --prefix)
    echo "-Wl,-rpath="$PYPREFIX"/lib"
}
