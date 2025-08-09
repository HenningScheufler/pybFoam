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

#include "bind_cfdTools.hpp"

#include "adjustPhi.H"
#include "findRefCell.H"
#include "constrainPressure.H"
#include "constrainHbyA.H"
#include "fvc.H"

namespace Foam
{

    std::tuple <scalar, scalar> computeCFLNumber
    (
        const surfaceScalarField& phi
    )
    {
        // Get the current time
        const fvMesh& mesh = phi.mesh();
        const Time& runTime = mesh.time();
        scalar CoNum = 0.0;
        scalar meanCoNum = 0.0;
        
        {
            scalarField sumPhi
            (
                fvc::surfaceSum(mag(phi))().primitiveField()
            );

            CoNum = 0.5*gMax(sumPhi/mesh.V().field())*runTime.deltaTValue();

            meanCoNum =
                0.5*(gSum(sumPhi)/gSum(mesh.V().field()))*runTime.deltaTValue();
        }

        return std::make_tuple(CoNum, meanCoNum);
    }
    

    template <typename RAUType>
    void declare_constrainPressure(pybind11::module &m)
    {
        // Declare the function in the module
        namespace py = pybind11;
        m.def("constrainPressure", [](volScalarField &p, const volVectorField &U, const surfaceScalarField &phiHbyA, const RAUType &rAU)
              { return constrainPressure(p, U, phiHbyA, rAU); }, py::arg("p"), py::arg("U"), py::arg("phiHbyA"), py::arg("rAU"));
    }

    void bindCfdTools(pybind11::module &m)
    {
        namespace py = pybind11;

        m.def("adjustPhi", &adjustPhi);
        declare_constrainPressure<volScalarField>(m);
        m.def("constrainHbyA", &constrainHbyA);
        m.def("createPhi", [](const volVectorField &U)
        {
            const fvMesh& mesh = U.mesh();
            surfaceScalarField phi
            (
                IOobject
                (
                    "phi",
                    mesh.time().timeName(),
                    mesh,
                    IOobject::READ_IF_PRESENT,
                    IOobject::AUTO_WRITE
                ),
                fvc::flux(U)
            );
            return phi;
        }, py::arg("U"));

        m.def("setRefCell", [](volScalarField &p, const Foam::dictionary &dict, const bool forceReference)
        {
            label pRefCell = 0;
            scalar pRefValue = 0.0;
            setRefCell(p, dict, pRefCell, pRefValue, forceReference);
            return std::make_tuple(pRefCell, pRefValue);
        }, py::arg("p"), py::arg("dict"), py::arg("forceReference") = false);
        m.def("computeCFLNumber", &computeCFLNumber);
    }

} // namespace Foam
