/*---------------------------------------------------------------------------*\
            Copyright (c) 2026, Henning Scheufler
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
    Header-only owner of the embedded Python interpreter.

    Manages the Py_Initialize/Py_Finalize lifecycle. Libraries that use
    nanobind must also call initEmbeddedNanobind() from nanobindEmbed.hpp.

    The class has hidden visibility, so every library that includes this
    header has a private copy. A single instance per Time is shared through
    the registry: it is looked up by name as a regIOobject, because a cast to
    pyInterp would fail for an instance created by another library. Only the
    instance that started the interpreter finalizes it.

    The library that creates the instance is pinned in memory, because the
    registry destroys the instance after OpenFOAM may have closed its
    runtime-loaded libraries.

Author
    Henning Scheufler

\*---------------------------------------------------------------------------*/

#ifndef pyInterp_H
#define pyInterp_H

#include <Python.h>
#include <dlfcn.h>

#include "Time.H"
#include "regIOobject.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class __attribute__((visibility("hidden"))) pyInterp
:
    public regIOobject
{
private:

    bool ownsInterpreter_;

    //- Keep the library containing this code loaded until process exit
    static void pinLibrary()
    {
        Dl_info info;
        if (dladdr(reinterpret_cast<void*>(&pyInterp::pinLibrary), &info))
        {
            // The handle is deliberately not closed
            dlopen(info.dli_fname, RTLD_NOW | RTLD_NOLOAD | RTLD_NODELETE);
        }
    }

public:

    //- Registry name of the interpreter
    static const char* registryName() noexcept
    {
        return "pyInterp";
    }

    // Constructors
    explicit pyInterp(const Time& time)
    :
        regIOobject
        (
            IOobject
            (
                registryName(),
                time.timeName(),
                time,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                false  // register object
            )
        ),
        ownsInterpreter_(!Py_IsInitialized())
    {
        pinLibrary();

        if (ownsInterpreter_)
        {
            Py_Initialize();

            // Add current working directory to sys.path so that local Python
            // modules (e.g. postProcess.py in the case directory) can be found.
            PyRun_SimpleString("import sys; sys.path.insert(0, '')");
        }

        Info<< "Starting Python Interpreter" << endl;
    }

    // Destructor
    virtual ~pyInterp()
    {
        if (ownsInterpreter_ && Py_IsInitialized())
        {
            Py_Finalize();
        }
    }

    // Selectors

    //- Return the interpreter of this Time, creating it if necessary
    static regIOobject& New(const Time& time)
    {
        regIOobject* ptr = time.getObjectPtr<regIOobject>(registryName());

        if (!ptr)
        {
            ptr = new pyInterp(time);

            ptr->store();
        }

        return *ptr;
    }

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
