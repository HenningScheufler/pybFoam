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

#include "bind_dict.hpp"
#include "OFstream.H"
#include "Fstream.H"
#include "IOobject.H"
#include "dictionaryEntry.H"
#include "entry.H"
#include <unordered_map>
#include <functional>

namespace Foam
{

    dictionary read_dictionary
    (
        const std::string& file_name
    )
    {
        autoPtr<IFstream> dictFile(new IFstream(file_name));
        if (!dictFile->good())
        {
            throw std::runtime_error("Could not open dictionary file: " + file_name);
        }
        dictionary dict(dictFile(), true);
        return dict;
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


class DictionaryGetProxy {
public:
    using GetterFunc = std::function<pybind11::object(Foam::dictionary&, const std::string&)>;
    static std::unordered_map<std::string, GetterFunc>& type_registry() {
        static std::unordered_map<std::string, GetterFunc> reg;
        return reg;
    }

    Foam::dictionary& dict;
    DictionaryGetProxy(Foam::dictionary& d) : dict(d) {}

    pybind11::object operator[](pybind11::object py_type) {
        std::string type_name = pybind11::str(py_type.attr("__name__"));
        auto& reg = type_registry();
        auto it = reg.find(type_name);
        if (it == reg.end())
            throw std::runtime_error("Unsupported type for dictionary.get: " + type_name);
        struct TypeCaller {
            Foam::dictionary& dict;
            GetterFunc func;
            TypeCaller(Foam::dictionary& d, GetterFunc f) : dict(d), func(f) {}
            pybind11::object operator()(const std::string& key) {
                if (!dict.found(key)) {
                    throw pybind11::key_error("Key '" + key + "' not found in dictionary");
                }
                return func(dict, key);
            }
        };
        return pybind11::cpp_function(TypeCaller(dict, it->second));
    }

    template<typename T>
    static void register_type(const std::string& py_name) {
        type_registry()[py_name] = [](Foam::dictionary& d, const std::string& key) {
            return pybind11::cast(d.get<T>(Foam::word(key)));
        };
    }
};

void bindDict(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<Foam::keyType>(m, "keyType")
    .def(py::init<const Foam::word &>())
    .def(py::init([](const std::string& str) {
        return new Foam::keyType(Foam::word(str));
    }))
    ;
    
    py::class_<Foam::entry>(m, "entry")
    ;

    // py::class_<Foam::dictionaryEntry, Foam::entry, Foam::dictionary>(m, "dictionaryEntry")
    // .def(py::init<const Foam::keyType &,const Foam::dictionary &,const Foam::dictionary &>())
    // ;
    

    py::class_<Foam::dictionary>(m, "dictionary")
        .def(py::init<const std::string&>())
        .def_static("read", [](const std::string& filename) -> Foam::dictionary* {
            // Allocate on heap and return pointer
            return new Foam::dictionary(Foam::read_dictionary(filename));
        }, py::return_value_policy::take_ownership)
        .def(py::init<>())
        .def(py::init<const Foam::dictionary&>())
        .def("toc", &Foam::dictionary::toc)
        .def("clear", &Foam::dictionary::clear)
        .def("clear", &Foam::dictionary::clear)
        .def("isDict", [](const Foam::dictionary& self, const std::string key)
        {
            return self.isDict(Foam::word(key));
        })
        .def("subDict", [](Foam::dictionary& self, const std::string key)
        {
            return static_cast<Foam::dictionary*>(&self.subDict(Foam::word(key)));
        },py::return_value_policy::reference_internal)
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
        .def_property_readonly(
            "get",
            [](Foam::dictionary& self) {
                return DictionaryGetProxy(self);
            },
            py::return_value_policy::reference_internal
        )
        ;

    py::class_<DictionaryGetProxy>(m, "DictionaryGetProxy")
        .def("__getitem__", &DictionaryGetProxy::operator[]);

    // Register types for get directly
    DictionaryGetProxy::type_registry()["str"] = [](Foam::dictionary& d, const std::string& key) {
        auto* entry = d.findEntry(Foam::word(key)); 
        if (entry->isStream())
        {
            return pybind11::cast(entry->stream().toString());
        }
        return pybind11::cast(static_cast<std::string>(d.get<Foam::word>(Foam::word(key))));
    };
    DictionaryGetProxy::register_type<Foam::scalar>("float");
    DictionaryGetProxy::register_type<Foam::label>("int");
    DictionaryGetProxy::register_type<bool>("bool");
    DictionaryGetProxy::register_type<Foam::word>("Word");
    DictionaryGetProxy::register_type<Foam::vector>("vector");
    DictionaryGetProxy::register_type<Foam::tensor>("tensor");
    DictionaryGetProxy::register_type<Foam::List<Foam::word>>("wordList");
    DictionaryGetProxy::register_type<Foam::Field<Foam::scalar>>("scalarField");
    DictionaryGetProxy::register_type<Foam::Field<Foam::vector>>("vectorField");
    DictionaryGetProxy::register_type<Foam::Field<Foam::tensor>>("tensorField");
}
