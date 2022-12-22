/*---------------------------------------------------------------------------*\
            Copyright (c) 2021, German Aerospace Center (DLR)
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

#include "pyInterp.H"

namespace py = pybind11;

namespace Foam
{
    defineTypeNameAndDebug(pyInterp, 0);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //
Foam::pyInterp::pyInterp(const Time& time)
:
    regIOobject
    (
        IOobject
        (
            pyInterp::typeName,
            time.timeName(),
            time,
            IOobject::NO_READ,
            IOobject::NO_WRITE,
            false  //register object
        )
    ),
    interp_()
{
    Info << "Starting Python Interpreter" << endl;; // use the Python API
}

Foam::pyInterp& Foam::pyInterp::New(const Time& time)
{
    pyInterp* ptr = time.getObjectPtr<pyInterp>
    (
        pyInterp::typeName
    );

    if (!ptr)
    {
        ptr = new pyInterp(time);

        ptr->store();
    }

    return *ptr;
}
// ************************************************************************* //
