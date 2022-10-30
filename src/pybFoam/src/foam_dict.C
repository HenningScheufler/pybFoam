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
    Type get(dictionary& dict, const std::string key)
    {
        return dict.get<Type>(word(key));
    }
    
    template<class Type>
    void set(dictionary& dict, const std::string key,const Type T)
    {
        dict.set<Type>(word(key),T);
    }

    template<class Type>
    void add(dictionary& dict, const std::string key,const Type T)
    {
        dict.add<Type>(word(key),T);
    }

}


void AddPyDict(pybind11::module& m)
{
    namespace py = pybind11;
    
    py::class_<Foam::entry>(m, "entry");

    py::class_<Foam::dictionary>(m, "dictionary")
        .def(py::init([](const std::string file_name) {
            return Foam::read_dictionary(file_name);
        }))
        .def(py::init([]() {
            return Foam::dictionary();
        }))
        .def(py::init([](const Foam::dictionary& d) {
            return Foam::dictionary(d);
        }))
        .def("toc", &Foam::dictionary::toc)
        .def("clear", &Foam::dictionary::clear)
        .def("isDict", [](const Foam::dictionary& self, const std::string key)
        {
            return self.isDict(Foam::word(key));
        })
        .def("subDict", [](const Foam::dictionary& self, const std::string key)
        {
            return self.subDict(Foam::word(key));
        })
        .def("get_word", &Foam::get<Foam::word>)
        .def("get_scalar", &Foam::get<Foam::scalar>)
        .def("get_vector", &Foam::get<Foam::vector>)
        .def("get_tensor", &Foam::get<Foam::tensor>)
        .def("get_wordList", &Foam::get<Foam::List<Foam::word>>)
        .def("get_scalarField", &Foam::get<Foam::Field<Foam::scalar>>)
        .def("get_vectorField", &Foam::get<Foam::Field<Foam::vector>>)
        .def("get_tensorField", &Foam::get<Foam::Field<Foam::tensor>>)

        .def("set", &Foam::set<Foam::word>)
        .def("set", &Foam::set<Foam::scalar>)
        .def("set", &Foam::set<Foam::vector>)
        .def("set", &Foam::set<Foam::tensor>)
        .def("set", &Foam::set<Foam::List<Foam::word>>)
        .def("set", &Foam::set<Foam::Field<Foam::scalar>>)
        .def("set", &Foam::set<Foam::Field<Foam::vector>>)
        .def("set", &Foam::set<Foam::Field<Foam::tensor>>)

        .def("add", &Foam::add<Foam::word>)
        .def("add", &Foam::add<Foam::scalar>)
        .def("add", &Foam::add<Foam::vector>)
        .def("add", &Foam::add<Foam::tensor>)
        .def("add", &Foam::add<Foam::List<Foam::word>>)
        .def("add", &Foam::add<Foam::Field<Foam::scalar>>)
        .def("add", &Foam::add<Foam::Field<Foam::vector>>)
        .def("add", &Foam::add<Foam::Field<Foam::tensor>>)
        ;
}
