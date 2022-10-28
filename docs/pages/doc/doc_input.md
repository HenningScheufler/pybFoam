---
title: Input
permalink: doc_input.html
keywords: Input
sidebar: doc_sidebar
folder: doc
---

## Usage

add following library to the controlDict

```
libs(externalComm)
```



## Boundary Conditions

add following entries to 0/\<field name\>

### flowRateInlet

```
inlet
{
    type coupledFlowRateInletVelocity;
    mFlowInit    1e-8;
    massFlowRate mdotin; // name of the input
    uniform (0 0 0);
}
```

### flowRateOutlet

```
inlet
{
    type coupledFlowRateOutletVelocity;
    mFlowInit    1e-8;
    massFlowRate mdotout; // name of the input
    uniform (0 0 0);
}
```

### uniformValue

```
inlet
{
    type coupledUniformExternalValue;
    inputName Tin;
    value uniform 350;
    initValue 293;
}
```

### wallHeatFlux

```
walls
{
    type            coupledWallHeatFluxTemperature;
    kappaMethod     lookup;
    kappa           DT;
    mode            power;
    QName           Qout;
    flipQ           true;
    Qinit           0;
    value           uniform 298;
}
```

## fvOptions

add following entries to constant/fvOptions



### coupledAccelerationSource

```
coupledAccelerationSource
{
    // Mandatory entries (unmodifiable)
    type             coupledAccelerationSource;

    accelerationName acc;
    omegaName omega;
    dOmegaDTName dOmegaDT;

    // Optional entries (unmodifiable)
    U                U;

}
```

## meshMotions

### coupledTranslationMotion

### externalCoupledForce