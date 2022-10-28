---
title: Output
permalink: doc_output.html
keywords: Output
sidebar: doc_sidebar
folder: doc
---

## Usage

add following library to the controlDict

```
libs(externalComm)
```


## One Dimensional

### extForces

```
extForces
{
    type extForces;
    patches (".*");
    CofR  (0 0 0);
    forceName force;
    momentName moment;
}
```

### extFunction

```
extFunction
{
    type extFunction;
    function constant 0;
    varName dTout;
}
```

### extPatch

not implented

### extSensor

```
extSensor
{
    type extSensor;
    sensorName Tout;
    fieldName T;
    sensorPosition (-0.01 0 0);
}
```
