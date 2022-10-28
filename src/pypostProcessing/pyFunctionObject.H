/*---------------------------------------------------------------------------*\
            Copyright (c) 2021, German Aerospace Center (DLR)
-------------------------------------------------------------------------------
License
    This file is part of the FMU4FOAM source code library, which is an
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
    Foam::pyFunctionObject

Description

Author
    Henning Scheufler, DLR, all rights reserved.

SourceFiles


\*---------------------------------------------------------------------------*/

#ifndef pyFunctionObject_H
#define pyFunctionObject_H

#include <pybind11/embed.h>
#include "typeInfo.H"
#include "word.H"
#include "Time.H"
#include "fvMesh.H"

namespace py = pybind11;
// using namespace py::literals;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam 
{

class pyFunctionObject
{
private:
    const fvMesh& mesh_;
    py::object pyFuncObj_;


public:

    //- Runtime type information
    TypeName("pyFunctionObject");

    // Constructors
    pyFunctionObject
    (
        const fvMesh& mesh,
        word pymodule,
        word pyclass
    );

    virtual ~pyFunctionObject() = default;
  
    bool execute()
    {
        pyFuncObj_.attr("execute")();
        return true;
    }
    
    bool end()
    {
        pyFuncObj_.attr("end")();
        return true;
    }

    bool write()
    {
        pyFuncObj_.attr("write")();
        return true;
    }

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
