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

namespace Foam
{

// Template helper functions for binding fvc operations

// Single argument operations (grad, div, laplacian, interpolate, snGrad, reconstruct, flux)
template<class FieldType>
void bindUnaryOp(pybind11::module& m, const char* opName)
{
    m.def(opName, [](const FieldType& vf){return fvc::grad(vf);});
    m.def(opName, [](const tmp<FieldType>& vf){return fvc::grad(vf);});
}

// Specialized template for grad operation
template<class FieldType>
void bindGrad(pybind11::module& m)
{
    m.def("grad", [](const FieldType& vf){return fvc::grad(vf);});
    m.def("grad", [](const tmp<FieldType>& vf){return fvc::grad(vf);});
}

// Specialized template for div operation
template<class FieldType>
void bindDiv(pybind11::module& m)
{
    m.def("div", [](const FieldType& vf){return fvc::div(vf);});
    m.def("div", [](const tmp<FieldType>& vf){return fvc::div(vf);});
}

// Specialized template for div-convection operation (2 arguments)
template<class FieldType>
void bindDivConvection(pybind11::module& m)
{
    m.def("div", [](const surfaceScalarField& ssf, const FieldType& vf){return fvc::div(ssf, vf);});
    m.def("div", [](const surfaceScalarField& ssf, const tmp<FieldType>& vf){return fvc::div(ssf, vf);});
}

// Specialized template for laplacian operation
template<class FieldType>
void bindLaplacian(pybind11::module& m)
{
    m.def("laplacian", [](const FieldType& vf){return fvc::laplacian(vf);});
    m.def("laplacian", [](const tmp<FieldType>& vf){return fvc::laplacian(vf);});
}

// Specialized template for laplacian with diffusivity (2 arguments)
template<class DiffType, class FieldType>
void bindLaplacianWithDiff(pybind11::module& m)
{
    // DiffType& + FieldType&
    m.def("laplacian", [](const DiffType& diff, const FieldType& vf){return fvc::laplacian(diff, vf);});
    // DiffType& + tmp<FieldType>
    m.def("laplacian", [](const DiffType& diff, const tmp<FieldType>& vf){return fvc::laplacian(diff, vf);});
    // tmp<DiffType> + FieldType&
    m.def("laplacian", [](const tmp<DiffType>& diff, const FieldType& vf){return fvc::laplacian(diff, vf);});
    // tmp<DiffType> + tmp<FieldType>
    m.def("laplacian", [](const tmp<DiffType>& diff, const tmp<FieldType>& vf){return fvc::laplacian(diff, vf);});
}

// Specialized template for interpolate operation
template<class FieldType>
void bindInterpolate(pybind11::module& m)
{
    m.def("interpolate", [](const FieldType& vf){return fvc::interpolate(vf);});
    m.def("interpolate", [](const tmp<FieldType>& vf){return fvc::interpolate(vf);});
}

// Specialized template for snGrad operation
template<class FieldType>
void bindSnGrad(pybind11::module& m)
{
    m.def("snGrad", [](const FieldType& vf){return fvc::snGrad(vf);});
    m.def("snGrad", [](const tmp<FieldType>& vf){return fvc::snGrad(vf);});
}

// Specialized template for reconstruct operation
template<class FieldType>
void bindReconstruct(pybind11::module& m)
{
    m.def("reconstruct", [](const FieldType& sf){return fvc::reconstruct(sf);});
    m.def("reconstruct", [](const tmp<FieldType>& sf){return fvc::reconstruct(sf);});
}

// Specialized template for flux operation
template<class FieldType>
void bindFlux(pybind11::module& m)
{
    m.def("flux", [](const FieldType& vf){return fvc::flux(vf);});
    m.def("flux", [](const tmp<FieldType>& vf){return fvc::flux(vf);});
}

// Specialized template for flux with surfaceScalarField (2 arguments)
template<class FieldType>
void bindFluxWithPhi(pybind11::module& m)
{
    m.def("flux", [](const surfaceScalarField& ssf, const FieldType& vf){return fvc::flux(ssf, vf);});
    m.def("flux", [](const surfaceScalarField& ssf, const tmp<FieldType>& vf){return fvc::flux(ssf, vf);});
}

} // End namespace Foam


void Foam::bindFVC(pybind11::module& fvc)
{
    namespace py = pybind11;

    // grad operations
    bindGrad<volScalarField>(fvc);
    bindGrad<volVectorField>(fvc);
    bindGrad<surfaceScalarField>(fvc);
    bindGrad<surfaceVectorField>(fvc);

    // div operations (single argument)
    bindDiv<volVectorField>(fvc);
    bindDiv<volTensorField>(fvc);
    bindDiv<volSymmTensorField>(fvc);
    bindDiv<surfaceScalarField>(fvc);
    bindDiv<surfaceVectorField>(fvc);
    bindDiv<surfaceTensorField>(fvc);
    bindDiv<surfaceSymmTensorField>(fvc);

    // div-convection operations (two arguments)
    bindDivConvection<volVectorField>(fvc);
    bindDivConvection<volTensorField>(fvc);
    bindDivConvection<volSymmTensorField>(fvc);

    // laplacian operations (single argument)
    bindLaplacian<volScalarField>(fvc);
    bindLaplacian<volVectorField>(fvc);
    bindLaplacian<volTensorField>(fvc);
    bindLaplacian<volSymmTensorField>(fvc);

    // laplacian with diffusivity (two arguments)
    // volScalar diffusivity
    bindLaplacianWithDiff<volScalarField, volScalarField>(fvc);
    bindLaplacianWithDiff<volScalarField, volVectorField>(fvc);
    bindLaplacianWithDiff<volScalarField, volTensorField>(fvc);
    bindLaplacianWithDiff<volScalarField, volSymmTensorField>(fvc);

    // surfaceScalar diffusivity
    bindLaplacianWithDiff<surfaceScalarField, volScalarField>(fvc);
    bindLaplacianWithDiff<surfaceScalarField, volVectorField>(fvc);
    bindLaplacianWithDiff<surfaceScalarField, volTensorField>(fvc);
    bindLaplacianWithDiff<surfaceScalarField, volSymmTensorField>(fvc);

    // volTensor diffusivity
    bindLaplacianWithDiff<volTensorField, volScalarField>(fvc);
    bindLaplacianWithDiff<volTensorField, volVectorField>(fvc);
    bindLaplacianWithDiff<volTensorField, volTensorField>(fvc);
    bindLaplacianWithDiff<volTensorField, volSymmTensorField>(fvc);

    // interpolate operations
    bindInterpolate<volScalarField>(fvc);
    bindInterpolate<volVectorField>(fvc);
    bindInterpolate<volTensorField>(fvc);
    bindInterpolate<volSymmTensorField>(fvc);

    // flux operations
    bindFlux<volVectorField>(fvc);
    bindFluxWithPhi<volVectorField>(fvc);

    // snGrad operations
    bindSnGrad<volScalarField>(fvc);
    bindSnGrad<volVectorField>(fvc);

    // reconstruct operations
    bindReconstruct<surfaceScalarField>(fvc);
    bindReconstruct<surfaceVectorField>(fvc);

    // ddtCorr (special case - single binding)
    fvc.def("ddtCorr", [](const volVectorField& vf, const surfaceScalarField& ssf){return fvc::ddtCorr(vf,ssf);});
}
