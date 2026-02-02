/*---------------------------------------------------------------------------*\
            Copyright (c) 2021-2026, German Aerospace Center (DLR)
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

#include "bind_blockmesh.hpp"
#include "mesh_utils.H"

#include "IOdictionary.H"
#include "blockMesh.H"
#include "fvMesh.H"
#include "polyMesh.H"
#include "IOstream.H"

#include <sstream>


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

Foam::fvMesh* Foam::generateBlockMesh
(
    Time& runTime,
    const dictionary& blockMeshDict,
    bool verbose,
    const std::string& timeName
)
{
    try
    {
        // Redirect output if not verbose
        MeshUtils::redirectOutput(verbose);

        // Create IOdictionary from regular dictionary
        // blockMesh requires an IOdictionary, not a plain dictionary
        IOdictionary ioMeshDict
        (
            IOobject
            (
                "blockMeshDict",
                runTime.system(),
                runTime,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                IOobject::NO_REGISTER
            ),
            blockMeshDict
        );

        // Create blockMesh object with default topology merge
        if (verbose)
        {
            Info<< "Creating block mesh from dictionary" << nl << endl;
        }

        blockMesh blocks(ioMeshDict, "region0", blockMesh::DEFAULT_MERGE, verbose);

        if (!blocks.good())
        {
            MeshUtils::restoreOutput();
            throw std::runtime_error("blockMesh: Did not generate any blocks");
        }

        // Clean old mesh files
        fileName polyMeshPath = runTime.path()/word(timeName)/"polyMesh";
        if (isDir(polyMeshPath))
        {
            if (verbose)
            {
                Info<< "Removing old polyMesh directory" << endl;
            }
            rmDir(polyMeshPath);
        }

        // Enable information messages
        blocks.verbose(verbose);

        // Generate the mesh
        if (verbose)
        {
            Info<< "Creating polyMesh from blockMesh" << nl << endl;
        }

        autoPtr<polyMesh> meshPtr = blocks.mesh
        (
            IOobject
            (
                "region0",
                word(timeName),
                runTime
            )
        );

        polyMesh& mesh = meshPtr();

        // Set precision for point data
        IOstream::minPrecision(10);

        // Write the mesh
        if (verbose)
        {
            Info<< nl << "Writing polyMesh" << endl;
        }

        mesh.removeFiles();
        bool sucessful_write = mesh.write();
        if (!sucessful_write)
        {
            MeshUtils::restoreOutput();
            throw std::runtime_error("Failed to write polyMesh");
        }

        // Clear the polyMesh and load fvMesh instead
        meshPtr.clear();

        if (verbose)
        {
            Info<< "Instantiating fvMesh from disk" << nl << endl;
        }

        fvMesh* fvMeshPtr = new fvMesh
        (
            IOobject
            (
                "region0",
                word(timeName),
                runTime,
                IOobject::MUST_READ
            ),
            false
        );
        fvMeshPtr->init(true);

        if (verbose)
        {
            Info<< nl << "End" << nl << endl;
        }

        // Restore output
        MeshUtils::restoreOutput();

        // Return the mesh pointer (ownership transferred to Python)
        return fvMeshPtr;
    }
    catch (const Foam::error& e)
    {
        MeshUtils::restoreOutput();
        std::ostringstream msg;
        msg << "OpenFOAM error in blockMesh: " << e.message().c_str();
        throw std::runtime_error(msg.str());
    }
    catch (const std::exception& e)
    {
        MeshUtils::restoreOutput();
        throw;
    }
}


void Foam::addBlockMeshBindings(py::module_& m)
{
    m.def("generate_blockmesh", &generateBlockMesh,
        py::arg("runtime"),
        py::arg("blockmesh_dict"),
        py::arg("verbose") = false,
        py::arg("time_name") = "constant",
        py::return_value_policy::take_ownership,
        R"pbdoc(
            Generate a block mesh from dictionary and return fvMesh.

            Parameters
            ----------
            runtime : Time
                OpenFOAM Time object for the case.
            blockmesh_dict : dictionary
                OpenFOAM dictionary with blockMeshDict format.
                Should contain: vertices, blocks, boundary.
            verbose : bool, optional
                Enable OpenFOAM output messages (default: False).
            time_name : str, optional
                Time directory for mesh output (default: "constant").

            Returns
            -------
            fvMesh
                The generated OpenFOAM fvMesh object.

            Raises
            ------
            RuntimeError
                If mesh generation fails.

            Examples
            --------
            >>> import pybFoam.mesh_generation as mg
            >>> from pybFoam.core import Time, dictionary
            >>>
            >>> # Create Time and dictionary
            >>> time = Time("/path/to/case")
            >>> mesh_dict = dictionary()
            >>>
            >>> # Generate mesh
            >>> mesh = mg.generate_blockmesh(time, mesh_dict, verbose=True)
            >>> print(f"Generated {mesh.nCells()} cells")
        )pbdoc"
    );
}


// ************************************************************************* //
