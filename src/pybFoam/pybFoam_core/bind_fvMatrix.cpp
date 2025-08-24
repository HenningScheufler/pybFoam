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

#include "bind_fvMatrix.hpp"
#include "tmp.H"


namespace Foam
{

template<class Type>
py::class_< fvMatrix<Type>>
declare_fvMatrix(py::module &m, std::string className)
{
    std::string tmp_fvMatrixClass = "tmp_" + className;
    py::class_< tmp<fvMatrix<Type>>>(m, tmp_fvMatrixClass.c_str())
        .def("__add__", []
        (
            const tmp<fvMatrix<Type>>& rhs,
            const tmp<fvMatrix<Type>>& lhs
        )
        {
            return rhs + lhs;
        })
        .def("__add__", []
        (
            const tmp<fvMatrix<Type>>& rhs,
            const fvMatrix<Type>& lhs
        )
        {
            return rhs + lhs;
        })
        .def("__add__", []
        (
            const tmp<fvMatrix<Type>>& rhs,
            const tmp<GeometricField<Type, fvPatchField, volMesh>>& lhs
        )
        {
            return rhs + lhs;
        })
        .def("__sub__", []
        (
            const tmp<fvMatrix<Type>>& rhs,
            const tmp<fvMatrix<Type>>& lhs
        )
        {
            return rhs - lhs;
        })
        .def("__sub__", []
        (
            const tmp<fvMatrix<Type>>& rhs,
            const fvMatrix<Type>& lhs
        )
        {
            return rhs - lhs;
        })
        .def("__sub__", []
        (
            const tmp<fvMatrix<Type>>& rhs,
            const tmp<GeometricField<Type, fvPatchField, volMesh>>& lhs
        )
        {
            return rhs - lhs;
        })
        ;

    ;

    auto fvMatrixClass = py::class_<fvMatrix<Type>>(m, className.c_str())
        .def(py::init<const fvMatrix<Type>&>())
        .def(py::init<tmp<fvMatrix<Type>>>())
        .def("solve", [](fvMatrix<Type> &self)
        {
            self.solve();
        })
        .def("solve", [](fvMatrix<Type> &self, const word& name)
        {
            self.solve(name);
        })
        .def("relax", [](fvMatrix<Type> &self, const scalar& alpha)
        {
            self.relax(alpha);
        })
        .def("relax", [](fvMatrix<Type> &self)
        {
            self.relax();
        })
        .def("setReference",&Foam::fvMatrix<Type>::setReference)
        .def("flux", &fvMatrix<Type>::flux)
        .def("A", &fvMatrix<Type>::A)
        .def("H", &fvMatrix<Type>::H)
        .def("D", &fvMatrix<Type>::D)
        .def("H1", &fvMatrix<Type>::H1)
        .def("__add__", []
        (
            const fvMatrix<Type>& rhs,
            const fvMatrix<Type>& lhs
        )
        {
            return rhs + lhs;
        })
        .def("__add__", []
        (
            const fvMatrix<Type>& rhs,
            const tmp<fvMatrix<Type>>& lhs
        )
        {
            return rhs + lhs;
        })
        .def("__add__", []
        (
            const fvMatrix<Type>& rhs,
            const tmp<GeometricField<Type, fvPatchField, volMesh>>& lhs
        )
        {
            return rhs + lhs;
        })
        .def("__sub__", []
        (
            const fvMatrix<Type>& rhs,
            const fvMatrix<Type>& lhs
        )
        {
            return rhs - lhs;
        })
        .def("__sub__", []
        (
            const fvMatrix<Type>& rhs,
            const tmp<fvMatrix<Type>>& lhs
        )
        {
            return rhs - lhs;
        })
        .def("__sub__", []
        (
            const fvMatrix<Type>& rhs,
            const tmp<GeometricField<Type, fvPatchField, volMesh>>& lhs
        )
        {
            return rhs - lhs;
        })
        ;

    return fvMatrixClass;

}

template<class Type>
void declare_solve(py::module &m)
{
    m.def("solve", [](fvMatrix<Type>& mat) {
        return Foam::solve(mat);
    });

    m.def("solve", [](const tmp<fvMatrix<Type>>& tmat) {
        return Foam::solve(tmat);
    });
}

}

void Foam::bindFvMatrix(py::module& m)
{
    namespace py = pybind11;

    auto fvScalarMatrix = declare_fvMatrix<Foam::scalar>(m, std::string("fvScalarMatrix"));
    auto fvVectorMatrix = declare_fvMatrix<Foam::vector>(m, std::string("fvVectorMatrix"));
    auto fvTensorMatrix = declare_fvMatrix<Foam::tensor>(m, std::string("fvTensorMatrix"));

    py::class_<Foam::SolverPerformance<Foam::scalar>>(m, "SolverScalarPerformance");
    py::class_<Foam::SolverPerformance<Foam::vector>>(m, "SolverVectorPerformance");
    py::class_<Foam::SolverPerformance<Foam::tensor>>(m, "SolverTensorPerformance");

    declare_solve<Foam::scalar>(m);
    declare_solve<Foam::vector>(m);
    declare_solve<Foam::tensor>(m);

}
