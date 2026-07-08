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
#include "gradScheme.H"
#include "divScheme.H"
#include "convectionScheme.H"
#include "snGradScheme.H"
#include "surfaceInterpolationScheme.H"
#include "IStringStream.H"
#include "dictionary.H"

#include <optional>
#include <stdexcept>

namespace nb = nanobind;

namespace Foam
{

// Every scheme-carrying operator accepts three optional, keyword-only args:
//   scheme=  inline spec (e.g. "Gauss upwind") -> built via <X>Scheme<Type>::New(mesh,[flux,]IStringStream)
//   key=     entry name looked up in the case's fvSchemes (OpenFOAM's native word overload)
//   key= + dict=  look the entry up in the caller-supplied dictionary instead of fvSchemes
// These mirror the OpenFOAM free function, which is <X>Scheme::New(mesh,[flux,]mesh.<x>Scheme(name)).
// dict is bound as a nullable pointer; dict->lookup(word) returns an ITstream& (an Istream).
static void checkSchemeArgs
(
    const std::optional<std::string>& scheme,
    const std::optional<std::string>& key,
    const dictionary* dict,
    const char* op
)
{
    if (scheme && (key || dict))
        throw std::invalid_argument(std::string(op) + ": 'scheme' is exclusive with 'key'/'dict'");
    if (dict && !key)
        throw std::invalid_argument(std::string(op) + ": 'dict' requires 'key'");
}

// Keyword-only arg tail shared by the folded scheme/key/dict defs.
#define SCHEME_KWARGS \
    nb::kw_only(), nb::arg("scheme") = nb::none(), \
    nb::arg("key") = nb::none(), nb::arg("dict").none() = nb::none()

// Template helper functions for binding fvc operations

// Single argument operations (grad, div, laplacian, interpolate, snGrad, reconstruct, flux)
template<class FieldType>
void bindUnaryOp(nanobind::module_& m, const char* opName)
{
    m.def(opName, [](const FieldType& vf){return fvc::grad(vf);});
    m.def(opName, [](const tmp<FieldType>& vf){return fvc::grad(vf);});
}

// grad — vol fields only (surface fields have no word/scheme overload upstream).
template<class Type>
void bindGradVol(nanobind::module_& m)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    auto impl = [](const Field& vf, std::optional<std::string> scheme,
                   std::optional<std::string> key, const dictionary* dict)
    {
        checkSchemeArgs(scheme, key, dict, "grad");
        if (scheme) { IStringStream is(*scheme);
            return fv::gradScheme<Type>::New(vf.mesh(), is)->grad(vf); }
        if (dict) return fv::gradScheme<Type>::New(vf.mesh(), dict->lookup(word(*key)))->grad(vf);
        if (key) return fvc::grad(vf, word(*key));
        return fvc::grad(vf);
    };
    m.def("grad", impl, nb::arg("vf"), SCHEME_KWARGS);
    m.def("grad", [impl](const tmp<Field>& vf, std::optional<std::string> scheme,
                         std::optional<std::string> key, const dictionary* dict)
        { return impl(vf(), scheme, key, dict); }, nb::arg("vf"), SCHEME_KWARGS);
}

// grad — plain (surface fields).
template<class FieldType>
void bindGrad(nanobind::module_& m)
{
    m.def("grad", [](const FieldType& vf){return fvc::grad(vf);});
    m.def("grad", [](const tmp<FieldType>& vf){return fvc::grad(vf);});
}

// div (1-arg divergence) — vol fields only.
template<class Type>
void bindDivVol(nanobind::module_& m)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    auto impl = [](const Field& vf, std::optional<std::string> scheme,
                   std::optional<std::string> key, const dictionary* dict)
    {
        checkSchemeArgs(scheme, key, dict, "div");
        if (scheme) { IStringStream is(*scheme);
            return fv::divScheme<Type>::New(vf.mesh(), is)->fvcDiv(vf); }
        if (dict) return fv::divScheme<Type>::New(vf.mesh(), dict->lookup(word(*key)))->fvcDiv(vf);
        if (key) return fvc::div(vf, word(*key));
        return fvc::div(vf);
    };
    m.def("div", impl, nb::arg("vf"), SCHEME_KWARGS);
    m.def("div", [impl](const tmp<Field>& vf, std::optional<std::string> scheme,
                        std::optional<std::string> key, const dictionary* dict)
        { return impl(vf(), scheme, key, dict); }, nb::arg("vf"), SCHEME_KWARGS);
}

// div (1-arg divergence) — plain (surface fields).
template<class FieldType>
void bindDiv(nanobind::module_& m)
{
    m.def("div", [](const FieldType& vf){return fvc::div(vf);});
    m.def("div", [](const tmp<FieldType>& vf){return fvc::div(vf);});
}

// div (convection, phi + field) — vol fields only.
template<class Type>
void bindDivConvection(nanobind::module_& m)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    auto impl = [](const surfaceScalarField& flux, const Field& vf, std::optional<std::string> scheme,
                   std::optional<std::string> key, const dictionary* dict)
    {
        checkSchemeArgs(scheme, key, dict, "div");
        if (scheme) { IStringStream is(*scheme);
            return fv::convectionScheme<Type>::New(vf.mesh(), flux, is)->fvcDiv(flux, vf); }
        if (dict) return fv::convectionScheme<Type>::New(vf.mesh(), flux, dict->lookup(word(*key)))->fvcDiv(flux, vf);
        if (key) return fvc::div(flux, vf, word(*key));
        return fvc::div(flux, vf);
    };
    m.def("div", impl, nb::arg("phi"), nb::arg("vf"), SCHEME_KWARGS);
    m.def("div", [impl](const surfaceScalarField& flux, const tmp<Field>& vf, std::optional<std::string> scheme,
                        std::optional<std::string> key, const dictionary* dict)
        { return impl(flux, vf(), scheme, key, dict); }, nb::arg("phi"), nb::arg("vf"), SCHEME_KWARGS);
}

// laplacian — key= only (delegates to the native word free function, which
// covers every gamma form including dimensioned<GType>).
template<class FieldType>
void bindLaplacian(nanobind::module_& m)
{
    m.def("laplacian", [](const FieldType& vf, std::optional<std::string> key)
        { if (key) return fvc::laplacian(vf, word(*key)); return fvc::laplacian(vf); },
        nb::arg("vf"), nb::kw_only(), nb::arg("key") = nb::none());
    m.def("laplacian", [](const tmp<FieldType>& vf, std::optional<std::string> key)
        { if (key) return fvc::laplacian(vf, word(*key)); return fvc::laplacian(vf); },
        nb::arg("vf"), nb::kw_only(), nb::arg("key") = nb::none());
}

// laplacian with diffusivity (2 arguments) — key= only.
template<class DiffType, class FieldType>
void bindLaplacianWithDiff(nanobind::module_& m)
{
    m.def("laplacian", [](const DiffType& diff, const FieldType& vf, std::optional<std::string> key)
        { if (key) return fvc::laplacian(diff, vf, word(*key)); return fvc::laplacian(diff, vf); },
        nb::arg("gamma"), nb::arg("vf"), nb::kw_only(), nb::arg("key") = nb::none());
    m.def("laplacian", [](const DiffType& diff, const tmp<FieldType>& vf, std::optional<std::string> key)
        { if (key) return fvc::laplacian(diff, vf, word(*key)); return fvc::laplacian(diff, vf); },
        nb::arg("gamma"), nb::arg("vf"), nb::kw_only(), nb::arg("key") = nb::none());
    m.def("laplacian", [](const tmp<DiffType>& diff, const FieldType& vf, std::optional<std::string> key)
        { if (key) return fvc::laplacian(diff, vf, word(*key)); return fvc::laplacian(diff, vf); },
        nb::arg("gamma"), nb::arg("vf"), nb::kw_only(), nb::arg("key") = nb::none());
    m.def("laplacian", [](const tmp<DiffType>& diff, const tmp<FieldType>& vf, std::optional<std::string> key)
        { if (key) return fvc::laplacian(diff, vf, word(*key)); return fvc::laplacian(diff, vf); },
        nb::arg("gamma"), nb::arg("vf"), nb::kw_only(), nb::arg("key") = nb::none());
}

// interpolate — vol fields.
template<class Type>
void bindInterpolate(nanobind::module_& m)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    auto impl = [](const Field& vf, std::optional<std::string> scheme,
                   std::optional<std::string> key, const dictionary* dict)
    {
        checkSchemeArgs(scheme, key, dict, "interpolate");
        if (scheme) { IStringStream is(*scheme);
            return surfaceInterpolationScheme<Type>::New(vf.mesh(), is)->interpolate(vf); }
        if (dict) return surfaceInterpolationScheme<Type>::New(vf.mesh(), dict->lookup(word(*key)))->interpolate(vf);
        if (key) return fvc::interpolate(vf, word(*key));
        return fvc::interpolate(vf);
    };
    m.def("interpolate", impl, nb::arg("vf"), SCHEME_KWARGS);
    m.def("interpolate", [impl](const tmp<Field>& vf, std::optional<std::string> scheme,
                                std::optional<std::string> key, const dictionary* dict)
        { return impl(vf(), scheme, key, dict); }, nb::arg("vf"), SCHEME_KWARGS);
}

// snGrad — vol fields.
template<class Type>
void bindSnGrad(nanobind::module_& m)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    auto impl = [](const Field& vf, std::optional<std::string> scheme,
                   std::optional<std::string> key, const dictionary* dict)
    {
        checkSchemeArgs(scheme, key, dict, "snGrad");
        if (scheme) { IStringStream is(*scheme);
            return fv::snGradScheme<Type>::New(vf.mesh(), is)->snGrad(vf); }
        if (dict) return fv::snGradScheme<Type>::New(vf.mesh(), dict->lookup(word(*key)))->snGrad(vf);
        if (key) return fvc::snGrad(vf, word(*key));
        return fvc::snGrad(vf);
    };
    m.def("snGrad", impl, nb::arg("vf"), SCHEME_KWARGS);
    m.def("snGrad", [impl](const tmp<Field>& vf, std::optional<std::string> scheme,
                           std::optional<std::string> key, const dictionary* dict)
        { return impl(vf(), scheme, key, dict); }, nb::arg("vf"), SCHEME_KWARGS);
}

// Specialized template for reconstruct operation
template<class FieldType>
void bindReconstruct(nanobind::module_& m)
{
    m.def("reconstruct", [](const FieldType& sf){return fvc::reconstruct(sf);});
    m.def("reconstruct", [](const tmp<FieldType>& sf){return fvc::reconstruct(sf);});
}

// Specialized template for flux operation (single argument)
template<class FieldType>
void bindFlux(nanobind::module_& m)
{
    m.def("flux", [](const FieldType& vf){return fvc::flux(vf);});
    m.def("flux", [](const tmp<FieldType>& vf){return fvc::flux(vf);});
}

// flux(phi, field, *, key=) — by-name fvSchemes lookup only (no inline factory
// for flux upstream).  Replaces the former positional-string flux overload.
template<class Type>
void bindFluxWithPhi(nanobind::module_& m)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    m.def("flux", [](const surfaceScalarField& ssf, const Field& vf, std::optional<std::string> key)
        { if (key) return fvc::flux(ssf, vf, word(*key)); return fvc::flux(ssf, vf); },
        nb::arg("phi"), nb::arg("vf"), nb::kw_only(), nb::arg("key") = nb::none());
    m.def("flux", [](const surfaceScalarField& ssf, const tmp<Field>& vf, std::optional<std::string> key)
        { if (key) return fvc::flux(ssf, vf(), word(*key)); return fvc::flux(ssf, vf()); },
        nb::arg("phi"), nb::arg("vf"), nb::kw_only(), nb::arg("key") = nb::none());
}

} // End namespace Foam


void Foam::bindFVC(nanobind::module_& fvc)
{
    // grad operations — vol fields carry scheme/key/dict; surface fields stay plain
    bindGradVol<scalar>(fvc);
    bindGradVol<vector>(fvc);
    bindGrad<surfaceScalarField>(fvc);
    bindGrad<surfaceVectorField>(fvc);

    // div operations (single argument) — vol fields carry scheme/key/dict
    bindDivVol<vector>(fvc);
    bindDivVol<tensor>(fvc);
    bindDivVol<symmTensor>(fvc);
    bindDiv<surfaceScalarField>(fvc);
    bindDiv<surfaceVectorField>(fvc);
    bindDiv<surfaceTensorField>(fvc);
    bindDiv<surfaceSymmTensorField>(fvc);

    // div-convection operations (two arguments)
    bindDivConvection<vector>(fvc);
    bindDivConvection<tensor>(fvc);
    bindDivConvection<symmTensor>(fvc);

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
    bindInterpolate<scalar>(fvc);
    bindInterpolate<vector>(fvc);
    bindInterpolate<tensor>(fvc);
    bindInterpolate<symmTensor>(fvc);

    // flux operations
    bindFlux<volVectorField>(fvc);
    bindFluxWithPhi<scalar>(fvc);
    bindFluxWithPhi<vector>(fvc);

    // snGrad operations
    bindSnGrad<scalar>(fvc);
    bindSnGrad<vector>(fvc);

    // reconstruct operations
    bindReconstruct<surfaceScalarField>(fvc);
    bindReconstruct<surfaceVectorField>(fvc);

    // ddtCorr (special case - single binding)
    fvc.def("ddtCorr", [](const volVectorField& vf, const surfaceScalarField& ssf){return fvc::ddtCorr(vf,ssf);});
}
