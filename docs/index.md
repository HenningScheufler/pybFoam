---
title: "ECI4FOAM"
keywords: sample homepage
sidebar: doc_sidebar
permalink: index.html
summary: ECI4FOAM with testsuite and documenation
---

External Coupling Interface 4 FOAM ECI4FOAM provides an interface for coupling external tools and software to OpenFOAM

## Installation

requires OpenFOAM of2012 or higher sourced and installed and python 3.7+

```
./build-ECI4FOAM.sh
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

{% include links.html %}


