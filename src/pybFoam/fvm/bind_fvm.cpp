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

namespace Foam
{

template <class Type>
void bindFvmDdt(py::module_& fvm)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    fvm.def("ddt", [](const Field& vf) { return fvm::ddt(vf); });
    fvm.def("ddt", [](const one&, const Field& vf) { return fvm::ddt(one{}, vf); });
    fvm.def("ddt", [](const dimensionedScalar& rho, const Field& vf) { return fvm::ddt(rho, vf); });
    fvm.def("ddt", [](const volScalarField& rho, const Field& vf) { return fvm::ddt(rho, vf); });
    fvm.def("ddt", [](const volScalarField& alpha,const volScalarField& rho, const Field& vf) { return fvm::ddt(alpha,rho, vf); });
}

template <class Type>
void bindFvmDiv(py::module_& fvm)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;
    fvm.def("div", [](const surfaceScalarField& flux, const Field& vf) { return fvm::div(flux, vf); });
    fvm.def("div", [](const tmp<surfaceScalarField>& flux, const Field& vf) { return fvm::div(flux, vf); });
}

template <class Type>
void bindFvmLaplacian(py::module_& fvm)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;

    fvm.def("laplacian", [](const Field& vf) { return fvm::laplacian(vf); });

    fvm.def("laplacian", [](const zero&, const Field& vf) { return fvm::laplacian(zero{}, vf); });
    fvm.def("laplacian", [](const one&, const Field& vf) { return fvm::laplacian(one{}, vf); });

    fvm.def("laplacian", [](const dimensionedScalar& gamma, const Field& vf) { return fvm::laplacian(gamma, vf); });

    fvm.def("laplacian", [](const volScalarField& gamma, const Field& vf) { return fvm::laplacian(gamma, vf); });
    fvm.def("laplacian", [](const tmp<volScalarField>& gamma, const Field& vf) { return fvm::laplacian(gamma, vf); });

    fvm.def("laplacian", [](const volTensorField& gamma, const Field& vf) { return fvm::laplacian(gamma, vf); });
    fvm.def("laplacian", [](const tmp<volTensorField>& gamma, const Field& vf) { return fvm::laplacian(gamma, vf); });

    fvm.def("laplacian", [](const surfaceScalarField& gamma, const Field& vf) { return fvm::laplacian(gamma, vf); });
    fvm.def("laplacian", [](const tmp<surfaceScalarField>& gamma, const Field& vf) { return fvm::laplacian(gamma, vf); });
}

template <class Type>
void bindFvmSources(py::module_& fvm)
{
    using Field = GeometricField<Type, fvPatchField, volMesh>;

    fvm.def("Su", [](const dimensioned<Type>& su, const Field& vf) { return fvm::Su(su, vf); });
    fvm.def("Su", [](const DimensionedField<Type, volMesh>& su, const Field& vf) { return fvm::Su(su, vf); });
    fvm.def("Su", [](const tmp<DimensionedField<Type, volMesh>>& su, const Field& vf) { return fvm::Su(su, vf); });
    fvm.def("Su", [](const tmp<Field>& su, const Field& vf) { return fvm::Su(su, vf); });

    fvm.def("Sp", [](const dimensionedScalar& sp, const Field& vf) { return fvm::Sp(sp, vf); });
    fvm.def("Sp", [](const DimensionedField<scalar, volMesh>& sp, const Field& vf) { return fvm::Sp(sp, vf); });
    fvm.def("Sp", [](const tmp<DimensionedField<scalar, volMesh>>& sp, const Field& vf) { return fvm::Sp(sp, vf); });
    fvm.def("Sp", [](const tmp<volScalarField>& sp, const Field& vf) { return fvm::Sp(sp, vf); });

    fvm.def("SuSp", [](const dimensionedScalar& susp, const Field& vf) { return fvm::SuSp(susp, vf); });
    fvm.def("SuSp", [](const DimensionedField<scalar, volMesh>& susp, const Field& vf) { return fvm::SuSp(susp, vf); });
    fvm.def("SuSp", [](const tmp<DimensionedField<scalar, volMesh>>& susp, const Field& vf) { return fvm::SuSp(susp, vf); });
    fvm.def("SuSp", [](const tmp<volScalarField>& susp, const Field& vf) { return fvm::SuSp(susp, vf); });
}

} // namespace Foam

void Foam::bindFVM(pybind11::module& fvm)
{
    namespace py = pybind11;
    // functions

    bindFvmDdt<scalar>(fvm);
    bindFvmDdt<vector>(fvm);
    bindFvmDdt<tensor>(fvm);

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
