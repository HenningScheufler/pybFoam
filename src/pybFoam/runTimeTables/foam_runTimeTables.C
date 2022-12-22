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

#include "foam_runTimeTables.H"
#include "Function1.H"
#include "fvPatchField.H"

namespace Foam
{
#define declareRunTimeSelectionTableToc(baseType,argNames)                    \
    const auto table = baseType::argNames##ConstructorTablePtr_->sortedToc(); \
    std::vector<std::string> table_out(table.size());                         \
    forAll(table,i)                                                           \
    {                                                                         \
        table_out[i] = table[i];                                              \
    }                                                                         \
    return table_out;                                                         \

}


void AddPyRunTime(pybind11::module& m)
{
    namespace py = pybind11;

    
    m.def("Function1", [](){
        declareRunTimeSelectionTableToc(Foam::Function1<Foam::scalar>, dictionary);
    })
    .def("fvPatchScalarField", [](){
        declareRunTimeSelectionTableToc(Foam::fvPatchScalarField, dictionary);
    })
    .def("fvPatchVectorField", [](){
        declareRunTimeSelectionTableToc(Foam::fvPatchVectorField, dictionary);
    })
    .def("fvPatchSphericalTensorField", [](){
        declareRunTimeSelectionTableToc(Foam::fvPatchSphericalTensorField, dictionary);
    })
    .def("fvPatchSymmTensorField", [](){
        declareRunTimeSelectionTableToc(Foam::fvPatchSymmTensorField, dictionary);
    })
    .def("fvPatchTensorField", [](){
        declareRunTimeSelectionTableToc(Foam::fvPatchTensorField, dictionary);
    })
    ;
   
}
