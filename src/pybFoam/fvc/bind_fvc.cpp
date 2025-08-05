/*---------------------------------------------------------------------------*\
            Copyright (c) 20212, Henning Scheufler
-------------------------------------------------------------------------------
License
    This file is part of the pybFoam source code library, which is an
	unofficial extension to OpenFOAM.
    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.
    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "bind_fvc.hpp"

#include "fvc.H"
#include "volFields.H"
#include "surfaceFields.H"


void Foam::bindFVC(pybind11::module& fvc)
{
    namespace py = pybind11;
    // functions

    // grad 
    fvc.def("grad", [](const volScalarField& vf){return fvc::grad(vf);});
    fvc.def("grad", [](const tmp<volScalarField>& vf){return fvc::grad(vf);});
    fvc.def("grad", [](const volVectorField& vf){return fvc::grad(vf);});
    fvc.def("grad", [](const tmp<volVectorField>& vf){return fvc::grad(vf);});
    fvc.def("grad", [](const surfaceScalarField& sf){return fvc::grad(sf);});
    fvc.def("grad", [](const tmp<surfaceScalarField>& sf){return fvc::grad(sf);});
    fvc.def("grad", [](const surfaceVectorField& sf){return fvc::grad(sf);});
    fvc.def("grad", [](const tmp<surfaceVectorField>& sf){return fvc::grad(sf);});

    // div
    fvc.def("div", [](const volVectorField& vf){return fvc::div(vf);});
    fvc.def("div", [](const tmp<volVectorField>& vf){return fvc::div(vf);});
    fvc.def("div", [](const volTensorField& vf){return fvc::div(vf);});
    fvc.def("div", [](const tmp<volTensorField>& vf){return fvc::div(vf);});
    fvc.def("div", [](const volSymmTensorField& vf){return fvc::div(vf);});
    fvc.def("div", [](const tmp<volSymmTensorField>& vf){return fvc::div(vf);});

    fvc.def("div", [](const surfaceScalarField& sf){return fvc::div(sf);});
    fvc.def("div", [](const tmp<surfaceScalarField>& sf){return fvc::div(sf);});
    fvc.def("div", [](const surfaceVectorField& sf){return fvc::div(sf);});
    fvc.def("div", [](const tmp<surfaceVectorField>& sf){return fvc::div(sf);});
    fvc.def("div", [](const surfaceTensorField& sf){return fvc::div(sf);});
    fvc.def("div", [](const tmp<surfaceTensorField>& sf){return fvc::div(sf);});
    fvc.def("div", [](const surfaceSymmTensorField& sf){return fvc::div(sf);});
    fvc.def("div", [](const tmp<surfaceSymmTensorField>& sf){return fvc::div(sf);});

    // div-convection
    fvc.def("div", [](const surfaceScalarField& ssf,const volVectorField& vf){return fvc::div(ssf,vf);});
    fvc.def("div", [](const surfaceScalarField& ssf,const tmp<volVectorField>& vf){return fvc::div(ssf,vf);});
    fvc.def("div", [](const surfaceScalarField& ssf,const volTensorField& vf){return fvc::div(ssf,vf);});
    fvc.def("div", [](const surfaceScalarField& ssf,const tmp<volTensorField>& vf){return fvc::div(ssf,vf);});
    fvc.def("div", [](const surfaceScalarField& ssf,const volSymmTensorField& vf){return fvc::div(ssf,vf);});
    fvc.def("div", [](const surfaceScalarField& ssf,const tmp<volSymmTensorField>& vf){return fvc::div(ssf,vf);});

    // laplacian
    fvc.def("laplacian", [](const volScalarField& vf){return fvc::laplacian(vf);});
    fvc.def("laplacian", [](const tmp<volScalarField>& vf){return fvc::laplacian(vf);});
    fvc.def("laplacian", [](const volVectorField& vf){return fvc::laplacian(vf);});
    fvc.def("laplacian", [](const tmp<volVectorField>& vf){return fvc::laplacian(vf);});
    fvc.def("laplacian", [](const volTensorField& vf){return fvc::laplacian(vf);});
    fvc.def("laplacian", [](const tmp<volTensorField>& vf){return fvc::laplacian(vf);});
    fvc.def("laplacian", [](const volSymmTensorField& vf){return fvc::laplacian(vf);});
    fvc.def("laplacian", [](const tmp<volSymmTensorField>& vf){return fvc::laplacian(vf);});

    // laplacian volScalar
    fvc.def("laplacian", [](const volScalarField& ssf,const volScalarField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const volScalarField& ssf,const tmp<volScalarField>& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const volScalarField& ssf,const volVectorField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const volScalarField& ssf,const tmp<volVectorField>& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const volScalarField& ssf,const volTensorField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const volScalarField& ssf,const tmp<volTensorField>& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const volScalarField& ssf,const volSymmTensorField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const volScalarField& ssf,const tmp<volSymmTensorField>& vf){return fvc::laplacian(ssf,vf);});

    fvc.def("laplacian", [](const tmp<volScalarField>& ssf,const volScalarField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const tmp<volScalarField>& ssf,const tmp<volScalarField>& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const tmp<volScalarField>& ssf,const volVectorField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const tmp<volScalarField>& ssf,const tmp<volVectorField>& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const tmp<volScalarField>& ssf,const volTensorField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const tmp<volScalarField>& ssf,const tmp<volTensorField>& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const tmp<volScalarField>& ssf,const volSymmTensorField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const tmp<volScalarField>& ssf,const tmp<volSymmTensorField>& vf){return fvc::laplacian(ssf,vf);});

    fvc.def("laplacian", [](const surfaceScalarField& ssf,const volScalarField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const surfaceScalarField& ssf,const tmp<volScalarField>& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const surfaceScalarField& ssf,const volVectorField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const surfaceScalarField& ssf,const tmp<volVectorField>& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const surfaceScalarField& ssf,const volTensorField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const surfaceScalarField& ssf,const tmp<volTensorField>& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const surfaceScalarField& ssf,const volSymmTensorField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const surfaceScalarField& ssf,const tmp<volSymmTensorField>& vf){return fvc::laplacian(ssf,vf);});

    // laplacian tensor
    fvc.def("laplacian", [](const volTensorField& ssf,const volScalarField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const volTensorField& ssf,const tmp<volScalarField>& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const volTensorField& ssf,const volVectorField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const volTensorField& ssf,const tmp<volVectorField>& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const volTensorField& ssf,const volTensorField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const volTensorField& ssf,const tmp<volTensorField>& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const volTensorField& ssf,const volSymmTensorField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const volTensorField& ssf,const tmp<volSymmTensorField>& vf){return fvc::laplacian(ssf,vf);});

    fvc.def("laplacian", [](const tmp<volTensorField>& ssf,const volScalarField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const tmp<volTensorField>& ssf,const tmp<volScalarField>& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const tmp<volTensorField>& ssf,const volVectorField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const tmp<volTensorField>& ssf,const tmp<volVectorField>& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const tmp<volTensorField>& ssf,const volTensorField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const tmp<volTensorField>& ssf,const tmp<volTensorField>& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const tmp<volTensorField>& ssf,const volSymmTensorField& vf){return fvc::laplacian(ssf,vf);});
    fvc.def("laplacian", [](const tmp<volTensorField>& ssf,const tmp<volSymmTensorField>& vf){return fvc::laplacian(ssf,vf);});

    // interpolate
    fvc.def("interpolate", [](const volScalarField& vf){return fvc::interpolate(vf);});
    fvc.def("interpolate", [](const tmp<volScalarField>& vf){return fvc::interpolate(vf);});
    fvc.def("interpolate", [](const volVectorField& vf){return fvc::interpolate(vf);});
    fvc.def("interpolate", [](const tmp<volVectorField>& vf){return fvc::interpolate(vf);});
    fvc.def("interpolate", [](const volTensorField& vf){return fvc::interpolate(vf);});
    fvc.def("interpolate", [](const tmp<volTensorField>& vf){return fvc::interpolate(vf);});
    fvc.def("interpolate", [](const volSymmTensorField& vf){return fvc::interpolate(vf);});
    fvc.def("interpolate", [](const tmp<volSymmTensorField>& vf){return fvc::interpolate(vf);});

    // flux
    fvc.def("flux", [](const volVectorField& vf){return fvc::flux(vf);});
    fvc.def("flux", [](const tmp<volVectorField>& vf){return fvc::flux(vf);});

    fvc.def("flux", [](const surfaceScalarField& ssf,const volVectorField& vf){return fvc::flux(ssf,vf);});
    fvc.def("flux", [](const surfaceScalarField& ssf,const tmp<volVectorField>& vf){return fvc::flux(ssf,vf);});

    // ddtCorr
    fvc.def("ddtCorr", [](const volVectorField& vf, const surfaceScalarField& ssf){return fvc::ddtCorr(vf,ssf);});
}
