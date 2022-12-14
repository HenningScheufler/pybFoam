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

#include "foam_dict.H"
#include "OFstream.H"
#include "Fstream.H"
#include "IOobject.H"
#include "dictionaryEntry.H"
#include "entry.H"
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

    py::class_<Foam::keyType>(m, "keyType")
    .def(py::init<const Foam::word &>())
    .def(py::init([](std::string k) {
        return Foam::keyType(Foam::word(k));
    }))
    ;
    
    py::class_<Foam::entry>(m, "entry")
    ;

    py::class_<Foam::dictionaryEntry, Foam::entry>(m, "dictionaryEntry")
    .def(py::init<const Foam::keyType &,const Foam::dictionary &,const Foam::dictionary &>())
    ;
    

    py::class_<Foam::dictionary>(m, "dictionary")
        .def(py::init([](const std::string file_name) {
            return Foam::dictionary(file_name);
        }))
        .def_static("read",[](const std::string file_name) {
            return Foam::read_dictionary(file_name);
        })
        .def(py::init([]() {
            return Foam::dictionary();
        }))
        .def(py::init([](const Foam::dictionary& d) {
            return Foam::dictionary(d);
        }))
        .def("toc", &Foam::dictionary::toc)
        .def("clear", &Foam::dictionary::clear)
        .def("clear", &Foam::dictionary::clear)
        .def("isDict", [](const Foam::dictionary& self, const std::string key)
        {
            return self.isDict(Foam::word(key));
        })
        .def("subDict", [](const Foam::dictionary& self, const std::string key)
        {
            return self.subDict(Foam::word(key));
        },py::return_value_policy::reference)
        .def("subDictOrAdd", [](Foam::dictionary& self, const std::string key)
        {
            return self.subDictOrAdd(Foam::word(key));
        },py::return_value_policy::reference_internal)
        .def("write", [](const Foam::dictionary& self,const std::string file_name)
        {
            Foam::fileName dictFileName(file_name);
            Foam::OFstream os(dictFileName);
            Foam::IOobject::writeBanner(os);
            Foam::IOobject::writeDivider(os);
            self.write(os,false);
            Foam::IOobject::writeEndDivider(os);
        })
        .def("print", [](const Foam::dictionary& self)
        {
            Foam::IOobject::writeBanner(Foam::Info);
            Foam::IOobject::writeDivider(Foam::Info);
            self.write(Foam::Info,false);
            Foam::IOobject::writeEndDivider(Foam::Info);
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

        .def("add", [](Foam::dictionary& self,const Foam::entry& e, bool merge)
        {
            self.add(e,merge);
        })
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
