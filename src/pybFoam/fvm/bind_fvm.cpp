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

#include "bind_fvm.hpp"

#include "fvm.H"
#include "convectionScheme.H"
#include "ddtScheme.H"
#include "laplacianScheme.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "ITstream.H"
#include "IStringStream.H"
#include "DynamicList.H"
#include "dictionary.H"

#include <optional>
#include <stdexcept>
#include <string_view>

namespace Foam
{

// Resolve the scheme token stream from the optional scheme/key/dict kwargs (see
// bind_fvc.cpp for the shared semantics: scheme=inline spec, key=fvSchemes entry
// name, dict=lookup source).  Returned by value so the inline case owns its
// freshly parsed tokens and the by-name cases copy the dict/mesh ITstream.
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
    {
        // Parse the inline scheme spec ("Gauss upwind", ...) into tokens.
        // ITstream's string-parsing constructors only exist in newer OpenFOAM;
        // tokenising through IStringStream and building the stream from a token
        // list works across all supported versions (v2312+).
        IStringStream iss(*scheme);
        DynamicList<token> toks;
        for (token t; (iss >> t, t.good()); )
            toks.append(t);

        tokenList tl;
        tl.transfer(toks);
        // Use a plain stream name: the spec string may contain spaces, which
        // OpenFOAM would reject when the name is validated as a fileName.
        return ITstream(word("scheme"), std::move(tl), IOstreamOption());
    }

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

// Per-scheme stream resolvers bound to the corresponding mesh lookup.
static ITstream ddtStream
(
    const std::optional<std::string>& scheme,
    const std::optional<std::string>& key,
    const dictionary* dict,
    const fvMesh& mesh,
    const word& defaultName
)
{
    return schemeStream(scheme, key, dict, defaultName,
        [&](const word& n) -> ITstream& { return mesh.ddtScheme(n); }, "ddt");
}

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

// ddt — runtime-selectable: every overload reduces to
//     ddtScheme<Type>::New(mesh, schemeStream(...))->fvmDdt(...)
template <class Type>
void bindFvmDdt(nb::module_& fvm)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    fvm.def("ddt", [](const Field& vf, std::optional<std::string> scheme,
                      std::optional<std::string> key, const dictionary* dict)
        {
            ITstream is = ddtStream(scheme, key, dict, vf.mesh(),
                word("ddt(" + vf.name() + ')'));
            return fv::ddtScheme<Type>::New(vf.mesh(), is)->fvmDdt(vf);
        },
        nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
    fvm.def("ddt", [](const dimensionedScalar& rho, const Field& vf,
                      std::optional<std::string> scheme,
                      std::optional<std::string> key, const dictionary* dict)
        {
            ITstream is = ddtStream(scheme, key, dict, vf.mesh(),
                word("ddt(" + rho.name() + ',' + vf.name() + ')'));
            return fv::ddtScheme<Type>::New(vf.mesh(), is)->fvmDdt(rho, vf);
        },
        nb::arg("rho"), nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
    fvm.def("ddt", [](const volScalarField& rho, const Field& vf,
                      std::optional<std::string> scheme,
                      std::optional<std::string> key, const dictionary* dict)
        {
            ITstream is = ddtStream(scheme, key, dict, vf.mesh(),
                word("ddt(" + rho.name() + ',' + vf.name() + ')'));
            return fv::ddtScheme<Type>::New(vf.mesh(), is)->fvmDdt(rho, vf);
        },
        nb::arg("rho"), nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
    fvm.def("ddt", [](const volScalarField& alpha, const volScalarField& rho, const Field& vf,
                      std::optional<std::string> scheme,
                      std::optional<std::string> key, const dictionary* dict)
        {
            ITstream is = ddtStream(scheme, key, dict, vf.mesh(),
                word("ddt(" + alpha.name() + ',' + rho.name() + ',' + vf.name() + ')'));
            return fv::ddtScheme<Type>::New(vf.mesh(), is)->fvmDdt(alpha, rho, vf);
        },
        nb::arg("alpha"), nb::arg("rho"), nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
}

// div(phi, field, *, scheme=, key=, dict=) — mirror of fvm::div, which is just
//   convectionScheme<Type>::New(mesh, flux, mesh.divScheme(name))->fvmDiv(flux, vf)
// so a scheme is fully described by its token stream and the kwargs only choose
// where that stream comes from (see bind_fvc.cpp for the shared semantics):
//   (default)  mesh.divScheme("div(phi,field)")  -- the default fvSchemes entry;
//   key=       mesh.divScheme(key)               -- a named fvSchemes entry;
//   dict=      dict.lookup(key)                  -- a named entry in a supplied dict;
//   scheme=    IStringStream(scheme)             -- inline spec (e.g. "Gauss upwind"),
//              used by the MULESCorr implicit-upwind predictor.
template <class Type>
void bindFvmDiv(nb::module_& fvm)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;

    auto impl = [](const surfaceScalarField& flux, const Field& vf,
                   std::optional<std::string> scheme,
                   std::optional<std::string> key, const dictionary* dict)
    {
        ITstream is = schemeStream(scheme, key, dict,
            word("div(" + flux.name() + ',' + vf.name() + ')'),
            [&](const word& n) -> ITstream& { return vf.mesh().divScheme(n); }, "div");
        return fv::convectionScheme<Type>::New(vf.mesh(), flux, is)->fvmDiv(flux, vf);
    };
    fvm.def("div", impl, nb::arg("phi"), nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
    fvm.def("div", [impl](const tmp<surfaceScalarField>& flux, const Field& vf,
                          std::optional<std::string> scheme,
                          std::optional<std::string> key, const dictionary* dict)
        { return impl(flux(), vf, scheme, key, dict); },
        nb::arg("phi"), nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
}

// laplacian with a field diffusivity — GType is the gamma field's element type:
//     laplacianScheme<Type, GType>::New(mesh, schemeStream(...))->fvmLaplacian(gamma, vf)
template <class GammaField, class Type>
void bindFvmLaplacianGamma(nb::module_& fvm)
{
    using GType = typename fieldElem<GammaField>::type;
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    auto impl = [](const GammaField& gamma, const Field& vf, std::optional<std::string> scheme,
                   std::optional<std::string> key, const dictionary* dict)
    {
        ITstream is = laplacianStream(scheme, key, dict, vf.mesh(),
            word("laplacian(" + gamma.name() + ',' + vf.name() + ')'));
        return fv::laplacianScheme<Type, GType>::New(vf.mesh(), is)->fvmLaplacian(gamma, vf);
    };
    fvm.def("laplacian", impl, nb::arg("gamma"), nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
    fvm.def("laplacian", [impl](const tmp<GammaField>& gamma, const Field& vf,
                                std::optional<std::string> scheme,
                                std::optional<std::string> key, const dictionary* dict)
        { return impl(gamma(), vf, scheme, key, dict); },
        nb::arg("gamma"), nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());
}

template <class Type>
void bindFvmLaplacian(nb::module_& fvm)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;

    // unit gamma: the scheme still needs a gamma, so materialise Gamma == 1
    // (mirroring fvm::laplacian(vf)) and drive laplacianScheme<Type, scalar>.
    fvm.def("laplacian", [](const Field& vf, std::optional<std::string> scheme,
                            std::optional<std::string> key, const dictionary* dict)
        {
            const surfaceScalarField Gamma
            (
                IOobject("1", vf.time().constant(), vf.mesh(), IOobject::NO_READ),
                vf.mesh(), dimensionedScalar("1", dimless, 1.0)
            );
            ITstream is = laplacianStream(scheme, key, dict, vf.mesh(),
                word("laplacian(" + vf.name() + ')'));
            return fv::laplacianScheme<Type, scalar>::New(vf.mesh(), is)->fvmLaplacian(Gamma, vf);
        },
        nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());

    // dimensionedScalar gamma: materialise a surface Gamma field first
    // (mirroring fvm::laplacian(dimensioned<GType>, vf)).
    fvm.def("laplacian", [](const dimensionedScalar& gamma, const Field& vf,
                            std::optional<std::string> scheme,
                            std::optional<std::string> key, const dictionary* dict)
        {
            const surfaceScalarField Gamma
            (
                IOobject(gamma.name(), vf.instance(), vf.mesh(), IOobject::NO_READ),
                vf.mesh(), gamma
            );
            ITstream is = laplacianStream(scheme, key, dict, vf.mesh(),
                word("laplacian(" + gamma.name() + ',' + vf.name() + ')'));
            return fv::laplacianScheme<Type, scalar>::New(vf.mesh(), is)->fvmLaplacian(Gamma, vf);
        },
        nb::arg("gamma"), nb::arg("vf"), nb::kw_only(),
        nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
        nb::arg("dict").none() = nb::none());

    // field diffusivities (vol/surface, scalar/tensor) with their tmp variants.
    bindFvmLaplacianGamma<volScalarField, Type>(fvm);
    bindFvmLaplacianGamma<volTensorField, Type>(fvm);
    bindFvmLaplacianGamma<surfaceScalarField, Type>(fvm);
}

template <class Type>
void bindFvmSources(nb::module_& fvm)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;

    fvm.def("Su", [](const dimensioned<Type>& su, const Field& vf) { return fvm::Su(su, vf); });
    // fvm.def("Su", [](const DimensionedField<Type, volMesh>& su, const Field& vf) { return fvm::Su(su, vf); });
    // fvm.def("Su", [](const tmp<DimensionedField<Type, volMesh>>& su, const Field& vf) { return fvm::Su(su, vf); });
    fvm.def("Su", [](const tmp<Field>& su, const Field& vf) { return fvm::Su(su, vf); });

    fvm.def("Sp", [](const dimensionedScalar& sp, const Field& vf) { return fvm::Sp(sp, vf); });
    // fvm.def("Sp", [](const DimensionedField<scalar, volMesh>& sp, const Field& vf) { return fvm::Sp(sp, vf); });
    // fvm.def("Sp", [](const tmp<DimensionedField<scalar, volMesh>>& sp, const Field& vf) { return fvm::Sp(sp, vf); });
    fvm.def("Sp", [](const tmp<volScalarField>& sp, const Field& vf) { return fvm::Sp(sp, vf); });

    fvm.def("SuSp", [](const dimensionedScalar& susp, const Field& vf) { return fvm::SuSp(susp, vf); });
    // fvm.def("SuSp", [](const DimensionedField<scalar, volMesh>& susp, const Field& vf) { return fvm::SuSp(susp, vf); });
    // fvm.def("SuSp", [](const tmp<DimensionedField<scalar, volMesh>>& susp, const Field& vf) { return fvm::SuSp(susp, vf); });
    fvm.def("SuSp", [](const tmp<volScalarField>& susp, const Field& vf) { return fvm::SuSp(susp, vf); });
}

} // namespace Foam

void Foam::bindFVM(nanobind::module_& fvm)
{    // functions

    bindFvmDdt<scalar>(fvm);
    bindFvmDdt<vector>(fvm);
    bindFvmDdt<tensor>(fvm);

    bindFvmDiv<scalar>(fvm);
    bindFvmDiv<vector>(fvm);
    bindFvmDiv<tensor>(fvm);
    bindFvmDiv<symmTensor>(fvm);

    bindFvmLaplacian<scalar>(fvm);
    bindFvmLaplacian<vector>(fvm);
    bindFvmLaplacian<tensor>(fvm);
    bindFvmLaplacian<symmTensor>(fvm);

    bindFvmSources<scalar>(fvm);
    bindFvmSources<vector>(fvm);
    bindFvmSources<tensor>(fvm);


}

// }
