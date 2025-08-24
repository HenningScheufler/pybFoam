/*---------------------------------------------------------------------------*\
            Copyright (c) 2022, Henning Scheufler
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
    Foam::bind_fvc

Description

Author
    Henning Scheufler, all rights reserved.

SourceFiles


\*---------------------------------------------------------------------------*/

#ifndef bind_fvc
#define bind_fvc

// System includes
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "word.H"
#include "scalar.H"
#include "vector.H"
#include "tensor.H"
#include "symmTensor.H"

namespace Foam
{

    void  bindFVC(pybind11::module& fvc);
}

#endif // bind_fvc  defined 
