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

Class
    Foam::pyInterp

Description
    Singleton Python interpreter for embedding pybFoam into OpenFOAM solvers.

    Manages Py_Initialize/Py_Finalize lifecycle and bootstraps nanobind's
    shared type registry so that nb::cast works for pybFoam types.

    Registered as a regIOobject on the Time registry to ensure exactly one
    interpreter exists per simulation.

Author
    Henning Scheufler, DLR, all rights reserved.

SourceFiles
    pyInterp.cpp

\*---------------------------------------------------------------------------*/

#ifndef pyInterp_H
#define pyInterp_H

#include <Python.h>
#include "typeInfo.H"
#include "word.H"
#include "Time.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class pyInterp
:
    public regIOobject
{
private:

    bool ownsInterpreter_;

public:

    //- Runtime type information
    TypeName("pyInterp");

    // Constructors
    pyInterp(const Time& time);

    // Destructor
    virtual ~pyInterp();

    // Selectors
    static pyInterp& New(const Time& time);

    // IO required by baseClass
    virtual bool writeData(Ostream&) const
    {
        return true;
    }

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
