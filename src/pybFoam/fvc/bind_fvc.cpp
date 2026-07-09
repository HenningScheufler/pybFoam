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
#include "laplacianScheme.H"
#include "ITstream.H"
#include "dictionary.H"

#include <optional>
#include <stdexcept>
#include <string_view>

namespace nb = nanobind;

namespace Foam
{

// A scheme is fully described by its token stream (an ITstream / Istream), so
// every scheme-carrying operator boils down to the single, branchless call
//     <X>Scheme<Type>::New(mesh, schemeStream(...))->op(...)
// exactly as the OpenFOAM free functions do internally.  The three optional,
// keyword-only args only choose where that stream comes from:
//   (default)     mesh.<x>Scheme(defaultName)  -- the op's default fvSchemes entry
//   key="entry"   mesh.<x>Scheme(key)          -- a named fvSchemes entry
//   dict=d        d.lookup(key)                -- a named entry in a supplied dict
//   scheme="..."  the inline spec parsed into tokens, e.g. "Gauss upwind"
// `meshScheme` is the per-operator mesh lookup (mesh.gradScheme, mesh.divScheme,
// mesh.snGradScheme, mesh.interpolationScheme, ...).  The stream is returned by
// value: for the inline spec it owns freshly parsed tokens; for the by-name
// forms it copies the ITstream held by the dict/mesh (a cheap token-list copy).
template<class MeshScheme>
static ITstream schemeStream
(
    const std::optional<std::string>& scheme,
    const std::optional<std::string>& key,
    const dictionary* dict,
    const word& defaultName,
    const MeshScheme& meshScheme,
    const char* op
)
{
    if (scheme && (key || dict))
        throw std::invalid_argument(std::string(op) + ": 'scheme' is exclusive with 'key'/'dict'");
    if (dict && !key)
        throw std::invalid_argument(std::string(op) + ": 'dict' requires 'key'");

    if (scheme)
        return ITstream(std::string_view(*scheme));

    const word name(key ? word(*key) : defaultName);
    ITstream is(dict ? dict->lookup(name) : meshScheme(name));
    is.rewind();
    return is;
}

// Element type of a GeometricField, used to pick laplacianScheme<Type, GType>
// from a gamma field type.
template<class GF> struct fieldElem;
template<class T, template<class> class PatchField, class Mesh>
struct fieldElem<GeometricField<T, PatchField, Mesh>> { using type = T; };

// laplacianScheme lookup bound to the mesh (mesh.laplacianScheme(name)).
static ITstream laplacianStream
(
    const std::optional<std::string>& scheme,
    const std::optional<std::string>& key,
    const dictionary* dict,
    const fvMesh& mesh,
    const word& defaultName
)
{
    return schemeStream(scheme, key, dict, defaultName,
        [&](const word& n) -> ITstream& { return mesh.laplacianScheme(n); }, "laplacian");
}

// grad — vol fields carry scheme/key/dict.
template<class Type>
void bindGradVol(nanobind::module_& m)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    auto impl = [](const Field& vf, std::optional<std::string> scheme,
                   std::optional<std::string> key, const dictionary* dict)
    {
        ITstream is = schemeStream(scheme, key, dict, word("grad(" + vf.name() + ')'),
            [&](const word& n) -> ITstream& { return vf.mesh().gradScheme(n); }, "grad");
        return fv::gradScheme<Type>::New(vf.mesh(), is)->grad(vf);
    };
    m.def("grad", impl, nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
    m.def("grad", [impl](const tmp<Field>& vf, std::optional<std::string> scheme,
                         std::optional<std::string> key, const dictionary* dict)
        { return impl(vf(), scheme, key, dict); },
        nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
}

// grad — plain (surface fields have no word/scheme overload upstream).
template<class FieldType>
void bindGrad(nanobind::module_& m)
{
    m.def("grad", [](const FieldType& vf){return fvc::grad(vf);});
    m.def("grad", [](const tmp<FieldType>& vf){return fvc::grad(vf);});
}

// div (1-arg divergence) — vol fields carry scheme/key/dict.
template<class Type>
void bindDivVol(nanobind::module_& m)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    auto impl = [](const Field& vf, std::optional<std::string> scheme,
                   std::optional<std::string> key, const dictionary* dict)
    {
        ITstream is = schemeStream(scheme, key, dict, word("div(" + vf.name() + ')'),
            [&](const word& n) -> ITstream& { return vf.mesh().divScheme(n); }, "div");
        return fv::divScheme<Type>::New(vf.mesh(), is)->fvcDiv(vf);
    };
    m.def("div", impl, nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
    m.def("div", [impl](const tmp<Field>& vf, std::optional<std::string> scheme,
                        std::optional<std::string> key, const dictionary* dict)
        { return impl(vf(), scheme, key, dict); },
        nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
}

// div (1-arg divergence) — plain (surface fields).
template<class FieldType>
void bindDiv(nanobind::module_& m)
{
    m.def("div", [](const FieldType& vf){return fvc::div(vf);});
    m.def("div", [](const tmp<FieldType>& vf){return fvc::div(vf);});
}

// div (convection, phi + field) — vol fields carry scheme/key/dict.
template<class Type>
void bindDivConvection(nanobind::module_& m)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    auto impl = [](const surfaceScalarField& phi, const Field& vf,
                   std::optional<std::string> scheme,
                   std::optional<std::string> key, const dictionary* dict)
    {
        ITstream is = schemeStream(scheme, key, dict,
            word("div(" + phi.name() + ',' + vf.name() + ')'),
            [&](const word& n) -> ITstream& { return vf.mesh().divScheme(n); }, "div");
        return fv::convectionScheme<Type>::New(vf.mesh(), phi, is)->fvcDiv(phi, vf);
    };
    m.def("div", impl, nb::arg("phi"), nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
    m.def("div", [impl](const surfaceScalarField& phi, const tmp<Field>& vf,
                        std::optional<std::string> scheme,
                        std::optional<std::string> key, const dictionary* dict)
        { return impl(phi, vf(), scheme, key, dict); },
        nb::arg("phi"), nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
}

// snGrad — vol fields carry scheme/key/dict.
template<class Type>
void bindSnGrad(nanobind::module_& m)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    auto impl = [](const Field& vf, std::optional<std::string> scheme,
                   std::optional<std::string> key, const dictionary* dict)
    {
        ITstream is = schemeStream(scheme, key, dict, word("snGrad(" + vf.name() + ')'),
            [&](const word& n) -> ITstream& { return vf.mesh().snGradScheme(n); }, "snGrad");
        return fv::snGradScheme<Type>::New(vf.mesh(), is)->snGrad(vf);
    };
    m.def("snGrad", impl, nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
    m.def("snGrad", [impl](const tmp<Field>& vf, std::optional<std::string> scheme,
                           std::optional<std::string> key, const dictionary* dict)
        { return impl(vf(), scheme, key, dict); },
        nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
}

// interpolate — vol fields carry scheme/key/dict.
template<class Type>
void bindInterpolate(nanobind::module_& m)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    auto impl = [](const Field& vf, std::optional<std::string> scheme,
                   std::optional<std::string> key, const dictionary* dict)
    {
        ITstream is = schemeStream(scheme, key, dict, word("interpolate(" + vf.name() + ')'),
            [&](const word& n) -> ITstream& { return vf.mesh().interpolationScheme(n); },
            "interpolate");
        return surfaceInterpolationScheme<Type>::New(vf.mesh(), is)->interpolate(vf);
    };
    m.def("interpolate", impl, nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
    m.def("interpolate", [impl](const tmp<Field>& vf, std::optional<std::string> scheme,
                                std::optional<std::string> key, const dictionary* dict)
        { return impl(vf(), scheme, key, dict); },
        nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
}

// laplacian (unit gamma) — runtime-selectable like the other schemes:
//     laplacianScheme<Type, scalar>::New(mesh, schemeStream(...))->fvcLaplacian(vf)
template<class Type>
void bindLaplacian(nanobind::module_& m)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    auto impl = [](const Field& vf, std::optional<std::string> scheme,
                   std::optional<std::string> key, const dictionary* dict)
    {
        ITstream is = laplacianStream(scheme, key, dict, vf.mesh(),
            word("laplacian(" + vf.name() + ')'));
        return fv::laplacianScheme<Type, scalar>::New(vf.mesh(), is)->fvcLaplacian(vf);
    };
    m.def("laplacian", impl, nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
    m.def("laplacian", [impl](const tmp<Field>& vf, std::optional<std::string> scheme,
                              std::optional<std::string> key, const dictionary* dict)
        { return impl(vf(), scheme, key, dict); },
        nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
}

// laplacian with a field diffusivity — GType is the gamma field's element type;
// both vol<GType> and surface<GType> gammas have an fvcLaplacian overload:
//     laplacianScheme<Type, GType>::New(mesh, schemeStream(...))->fvcLaplacian(gamma, vf)
template<class Type, class GammaField>
void bindLaplacianWithDiff(nanobind::module_& m)
{
    using GType = typename fieldElem<GammaField>::type;
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    auto impl = [](const GammaField& gamma, const Field& vf, std::optional<std::string> scheme,
                   std::optional<std::string> key, const dictionary* dict)
    {
        ITstream is = laplacianStream(scheme, key, dict, vf.mesh(),
            word("laplacian(" + gamma.name() + ',' + vf.name() + ')'));
        return fv::laplacianScheme<Type, GType>::New(vf.mesh(), is)->fvcLaplacian(gamma, vf);
    };
    m.def("laplacian", impl, nb::arg("gamma"), nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
    m.def("laplacian", [impl](const tmp<GammaField>& gamma, const Field& vf,
                              std::optional<std::string> scheme,
                              std::optional<std::string> key, const dictionary* dict)
        { return impl(gamma(), vf, scheme, key, dict); },
        nb::arg("gamma"), nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
    m.def("laplacian", [impl](const GammaField& gamma, const tmp<Field>& vf,
                              std::optional<std::string> scheme,
                              std::optional<std::string> key, const dictionary* dict)
        { return impl(gamma, vf(), scheme, key, dict); },
        nb::arg("gamma"), nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
    m.def("laplacian", [impl](const tmp<GammaField>& gamma, const tmp<Field>& vf,
                              std::optional<std::string> scheme,
                              std::optional<std::string> key, const dictionary* dict)
        { return impl(gamma(), vf(), scheme, key, dict); },
        nb::arg("gamma"), nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
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

// flux(phi, field, *, key=) — by-name fvSchemes lookup only (no inline scheme
// factory for flux upstream).
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

    // laplacian operations (single argument, unit gamma)
    bindLaplacian<scalar>(fvc);
    bindLaplacian<vector>(fvc);
    bindLaplacian<tensor>(fvc);
    bindLaplacian<symmTensor>(fvc);

    // laplacian with diffusivity (two arguments) — <fieldType, gammaFieldType>
    // volScalar diffusivity
    bindLaplacianWithDiff<scalar, volScalarField>(fvc);
    bindLaplacianWithDiff<vector, volScalarField>(fvc);
    bindLaplacianWithDiff<tensor, volScalarField>(fvc);
    bindLaplacianWithDiff<symmTensor, volScalarField>(fvc);

    // surfaceScalar diffusivity
    bindLaplacianWithDiff<scalar, surfaceScalarField>(fvc);
    bindLaplacianWithDiff<vector, surfaceScalarField>(fvc);
    bindLaplacianWithDiff<tensor, surfaceScalarField>(fvc);
    bindLaplacianWithDiff<symmTensor, surfaceScalarField>(fvc);

    // volTensor diffusivity
    bindLaplacianWithDiff<scalar, volTensorField>(fvc);
    bindLaplacianWithDiff<vector, volTensorField>(fvc);
    bindLaplacianWithDiff<tensor, volTensorField>(fvc);
    bindLaplacianWithDiff<symmTensor, volTensorField>(fvc);

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
