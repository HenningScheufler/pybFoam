/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2022 AUTHOR,AFFILIATION
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

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

Application
    test-pythonNumpy

Description

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "pyInterp.H"

#include "sigFpe.H"
#include "sigInt.H"
#include "sigQuit.H"
#include "sigSegv.H"
namespace py = pybind11;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    runTime.printExecutionTime(Info);

    pyInterp::New(runTime);
    sigFpe::unset(true);

    py::object pyC = py::module_::import("testing").attr("Testing");

    sigFpe::set(true);

    pyC("test-numpy: Hello");

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
