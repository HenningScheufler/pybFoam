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
#include "IStringStream.H"
#include "dictionary.H"

#include <optional>
#include <stdexcept>

namespace Foam
{

// Validate the optional scheme/key/dict kwargs (see bind_fvc.cpp for the shared
// semantics: scheme=inline spec, key=fvSchemes entry name, dict=lookup source).
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

template <class Type>
void bindFvmDdt(nb::module_& fvm)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    fvm.def("ddt", [](const Field& vf) { return fvm::ddt(vf); });
    // fvm.def("ddt", [](const one&, const Field& vf) { return fvm::ddt(one{}, vf); });
    fvm.def("ddt", [](const dimensionedScalar& rho, const Field& vf) { return fvm::ddt(rho, vf); });
    fvm.def("ddt", [](const volScalarField& rho, const Field& vf) { return fvm::ddt(rho, vf); });
    fvm.def("ddt", [](const volScalarField& alpha,const volScalarField& rho, const Field& vf) { return fvm::ddt(alpha,rho, vf); });
}

template <class Type>
void bindFvmDiv(nb::module_& fvm)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    fvm.def("div", [](const tmp<surfaceScalarField>& flux, const Field& vf) { return fvm::div(flux, vf); });

    // div(phi, field, *, scheme=, key=, dict=) — mirror of fvm::div; the three
    // optional kwargs swap the Istream source fed to convectionScheme::New:
    //   scheme= inline spec (e.g. "Gauss upwind"), independent of fvSchemes
    //           (used by the MULESCorr implicit-upwind predictor);
    //   key=    entry looked up in the case's fvSchemes (native word overload);
    //   key= + dict=  entry looked up in the caller-supplied dictionary.
    fvm.def("div", [](const surfaceScalarField& flux, const Field& vf, std::optional<std::string> scheme,
                      std::optional<std::string> key, const dictionary* dict)
        {
            checkSchemeArgs(scheme, key, dict, "div");
            if (scheme) { IStringStream is(*scheme);
                return fv::convectionScheme<Type>::New(vf.mesh(), flux, is)->fvmDiv(flux, vf); }
            if (dict) return fv::convectionScheme<Type>::New(vf.mesh(), flux, dict->lookup(word(*key)))->fvmDiv(flux, vf);
            if (key) return fvm::div(flux, vf, word(*key));
            return fvm::div(flux, vf);
        }, nb::arg("phi"), nb::arg("vf"), nb::kw_only(),
           nb::arg("scheme") = nb::none(), nb::arg("key") = nb::none(),
           nb::arg("dict").none() = nb::none());
}

// laplacian gamma form with an optional key= (fvSchemes lookup); scheme=/dict=
// are deferred (the dimensioned<GType> gamma materialises a Gamma field).
#define FVM_LAPLACIAN_KEY(GAMMA_T) \
    fvm.def("laplacian", [](GAMMA_T gamma, const Field& vf, std::optional<std::string> key) \
        { if (key) return fvm::laplacian(gamma, vf, word(*key)); return fvm::laplacian(gamma, vf); }, \
        nb::arg("gamma"), nb::arg("vf"), nb::kw_only(), nb::arg("key") = nb::none())

template <class Type>
void bindFvmLaplacian(nb::module_& fvm)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;

    fvm.def("laplacian", [](const Field& vf, std::optional<std::string> key)
        { if (key) return fvm::laplacian(vf, word(*key)); return fvm::laplacian(vf); },
        nb::arg("vf"), nb::kw_only(), nb::arg("key") = nb::none());

    FVM_LAPLACIAN_KEY(const dimensionedScalar&);
    FVM_LAPLACIAN_KEY(const volScalarField&);
    FVM_LAPLACIAN_KEY(const tmp<volScalarField>&);
    FVM_LAPLACIAN_KEY(const volTensorField&);
    FVM_LAPLACIAN_KEY(const tmp<volTensorField>&);
    FVM_LAPLACIAN_KEY(const surfaceScalarField&);
    FVM_LAPLACIAN_KEY(const tmp<surfaceScalarField>&);
}
#undef FVM_LAPLACIAN_KEY

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
