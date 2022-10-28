---
title: External Communication
permalink: doc_externalComm.html
keywords: design
sidebar: doc_sidebar
folder: doc
---

The main concept of the externalComm library is similar to the FMI Standard and it can be viewed as blackbox with time-dependent input and output values and parameters that are fixed in time.

# Usage


## add to OpenFOAM
add following library to the controlDict

```
libs(externalComm)
```

## input

```cpp
    // create new entry
    const Time& runTime = mesh_.time(); // we need the singleton time
    commDataLayer& data = commDataLayer::New(runTime);

    scalar varName = "varName";
    scalar varValue = "varValue";

    data.storeObj(varValue,varName,commDataLayer::causality::in);
```

```cpp
    // work with data
    commDataLayer& data = commDataLayer::New(runTime);
    // get varName from registry
    scalar& val = data.getObj<scalar>("varName",commDataLayer::causality::in);
    val = newValue;
```

## output

```cpp
    // create new entry
    const Time& runTime = mesh_.time(); // we need the singleton time
    commDataLayer& data = commDataLayer::New(runTime);

    scalar varName2 = "varName2"; 
    
    vector varValue2(0,0,0); 
    data.storeObj(varValue2,varName2,commDataLayer::causality::out);
```

```cpp
    // work with data
    commDataLayer& data = commDataLayer::New(runTime);
    // get varName2 from registry
    vector& val2 = data.getObj<vector>("varName2",commDataLayer::causality::out); 
    val2 = newValue;
```

# Implementation

## Data

As hinted above the approach is similar to FMI standard with the categorization of variables in 3 causality:

- input -> changes in time (and space)
- output -> changes in time (and space)
- parameter -> constant in time (and space)

The core class `commDataLayer` creates 3 `objectRegistries` one for each causality that are accessible with an enum:

- `commDataLayer::causality::in`
- `commDataLayer::causality::parameter`
- `commDataLayer::causality::out`


Each of these `objectRegistries` store and manage the objects passed to them. The elements in the objectsRegistry can be accessed similar to a hashtable. The data is stored in the template class `registeredObject<Type>` which currently features following Types:

- Primitive Types:
    - `bool`    -> FMI 2.0
    - `word`    -> FMI 2.0
    - `label`   -> FMI 2.0
    - `scalar`  -> FMI 2.0
    - `vector`
    - `symmTensor`
    - `sphericalTensor`
    - `tensor`

- Fields:
    - `Field<scalar>`
    - `Field<vector>`
    - `Field<symmTensor>`
    - `Field<sphericalTensor>`
    - `Field<tensor>`

## Comm

### Websocket


### Zmq


### grpc

## Json



## example