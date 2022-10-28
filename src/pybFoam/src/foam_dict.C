/*---------------------------------------------------------------------------*\
            Copyright (c) 20212, Henning Scheufler
-------------------------------------------------------------------------------
License
    This file is part of the ECI4FOAM source code library, which is an
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

#include "foam_dict.H"


class Dict {
    private:
        Foam::dictionary dict_;


    public:
    Dict(const std::string &file_name)
    :
    dict_()
    {
        Foam::autoPtr<Foam::IFstream> dictFile(new Foam::IFstream(file_name));
        dict_ = Foam::dictionary(dictFile(), true);
    }

    std::vector<std::string> toc()
    {
        std::vector<std::string> content = {};
        for (const Foam::word& w: dict_.toc())
        {
            content.push_back(w);
        }
        return content;
    }

    std::string value(std::string scopedName)
    {

        // const auto finder = dict_.csearchScoped(scopedName, Foam::keyType::REGEX);

        // if (!finder.found())
        // {
        //     FatalIOErrorInFunction(dict_.name())
        //         << "Cannot find entry " << scopedName
        //         << exit(Foam::FatalIOError, 2);
        // }

        // if (finder.isDict())
        // {
        //     Foam::Info << finder.dict();
        // }
        // else if (finder.ref().isStream())
        // {
        //     const Foam::tokenList& tokens = finder.ref().stream();
        //     Foam::IStringStream istr();
        //     forAll(tokens, i)
        //     {
        //         Foam::Info<< tokens[i];
        //         if (i < tokens.size() - 1)
        //         {
        //             Foam::Info<< Foam::token::SPACE;
        //         }
        //     }
        //     Foam::Info << Foam::endl;
        //     // return istr.str();
        //     return std::string("asdasdasd");
        // }
        return std::string("asdasdasd");
    
    }
};

namespace Foam
{

    dictionary read_dictionary
    (
        const std::string& file_name
    )
    {
        autoPtr<IFstream> dictFile(new IFstream(file_name));
        return dictionary(dictFile(), true);
    }

    template<class Type>
    Type getFromDict(dictionary& dict, const std::string key)
    {
        return dict.get<Type>(word(key));
    }

}


void AddPyDict(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<Foam::dictionary>(m, "dictionary")
        .def(py::init([](const std::string name) {
            return Foam::read_dictionary(name);
        }))
        .def("toc", &Foam::dictionary::toc)
        .def("get_word", &Foam::getFromDict<Foam::word>)
        .def("get_scalar", &Foam::getFromDict<Foam::scalar>)
        .def("get_vector", &Foam::getFromDict<Foam::vector>)
        .def("get_tensor", &Foam::getFromDict<Foam::tensor>)
        .def("get_wordList", &Foam::getFromDict<Foam::List<Foam::word>>)
        .def("get_scalarField", &Foam::getFromDict<Foam::Field<Foam::scalar>>)
        .def("get_vectorField", &Foam::getFromDict<Foam::Field<Foam::vector>>)
        .def("get_tensorField", &Foam::getFromDict<Foam::Field<Foam::tensor>>)
        ;
}
