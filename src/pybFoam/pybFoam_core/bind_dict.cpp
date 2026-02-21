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
#include "SolverPerformance.H"
#include "List.H"
#include <unordered_map>
#include <functional>

namespace nb = nanobind;

namespace Foam
{

    dictionary read_dictionary(
        const std::string &file_name)
    {
        autoPtr<IFstream> dictFile(new IFstream(file_name));
        if (!dictFile->good())
        {
            throw std::runtime_error("Could not open dictionary file: " + file_name);
        }
        dictionary dict(dictFile(), true);
        return dict;
    }

    template <class Type>
    Type get(dictionary &dict, const std::string key)
    {
        return dict.get<Type>(word(key));
    }

    template <class Type>
    void set(dictionary &dict, const std::string key, const Type T)
    {
        dict.set<Type>(word(key), T);
    }

    template <class Type>
    void add(dictionary &dict, const std::string key, const Type T)
    {
        dict.add<Type>(word(key), T);
    }

    // Helper function to lookup List<SolverPerformance<Type>> and return as std::vector
    // OpenFOAM stores solver performance as List<SolverPerformance<Type>>
    template <class Type>
    std::vector<SolverPerformance<Type>> lookupSolverPerformanceList(const dictionary &dict, const std::string &fieldName)
    {
        // Check if entry exists first
        const entry *e = dict.findEntry(word(fieldName), keyType::LITERAL);
        if (!e)
        {
            throw std::runtime_error(
                "Entry " + fieldName + " not found in dictionary");
        }

        // Enable exception throwing for FatalIOError to catch type mismatches
        bool previousThrowState = FatalIOError.throwExceptions();

        try
        {
            // The dictionary stores List<SolverPerformance<Type>>
            List<SolverPerformance<Type>> sp;
            bool success = dict.readIfPresent(word(fieldName), sp);

            // Restore previous exception state
            FatalIOError.throwExceptions(previousThrowState);

            if (!success || sp.empty())
            {
                throw std::runtime_error(
                    "Entry " + fieldName + " could not be read as List<SolverPerformance<" + pTraits<Type>::typeName + ">> type or is empty");
            }

            // Convert to std::vector for Python compatibility
            std::vector<SolverPerformance<Type>> result;
            result.reserve(sp.size());
            for (const auto &item : sp)
            {
                result.push_back(item);
            }

            return result;
        }
        catch (const Foam::error &e)
        {
            // Restore previous exception state before rethrowing
            FatalIOError.throwExceptions(previousThrowState);

            throw std::runtime_error(
                "Entry " + fieldName + " could not be read as List<SolverPerformance<" + pTraits<Type>::typeName + ">> type: " + std::string(e.what()));
        }
        catch (...)
        {
            // Restore previous exception state before rethrowing
            FatalIOError.throwExceptions(previousThrowState);
            throw;
        }
    }

}

class DictionaryGetProxy
{
public:
    using GetterFunc = std::function<nb::object(Foam::dictionary &, const std::string &)>;
    static std::unordered_map<std::string, GetterFunc> &type_registry()
    {
        static std::unordered_map<std::string, GetterFunc> reg;
        return reg;
    }

    Foam::dictionary &dict;
    DictionaryGetProxy(Foam::dictionary &d) : dict(d) {}

    nb::object operator[](nb::object py_type)
    {
        std::string type_name = nb::cast<std::string>(py_type.attr("__name__"));
        auto &reg = type_registry();
        auto it = reg.find(type_name);
        if (it == reg.end())
            throw std::runtime_error("Unsupported type for dictionary.get: " + type_name);

        struct TypeCaller
        {
            Foam::dictionary &dict;
            GetterFunc func;
            TypeCaller(Foam::dictionary &d, GetterFunc f) : dict(d), func(f) {}

            nb::object operator()(const std::string &key) const
            {
                if (!dict.found(key))
                {
                    throw nb::key_error(("Key '" + key + "' not found in dictionary").c_str());
                }
                return func(dict, key);
            }
        };

        return nb::cpp_function(TypeCaller(dict, it->second));
    }

    template <typename T>
    static void register_type(const std::string &py_name)
    {
        type_registry()[py_name] = [](Foam::dictionary &d, const std::string &key)
        {
            return nb::cast(d.get<T>(Foam::word(key)));
        };
    }
};

class DictionaryGetOrDefaultProxy
{
public:
    using GetterFunc = std::function<nb::object(Foam::dictionary &, const std::string &)>;

    Foam::dictionary &dict;
    DictionaryGetOrDefaultProxy(Foam::dictionary &d) : dict(d) {}

    nb::object operator[](nb::object py_type)
    {
        std::string type_name = nb::cast<std::string>(py_type.attr("__name__"));
        auto &reg = DictionaryGetProxy::type_registry();
        auto it = reg.find(type_name);
        if (it == reg.end())
            throw std::runtime_error("Unsupported type for dictionary.getOrDefault: " + type_name);

        struct TypeCaller
        {
            Foam::dictionary &dict;
            GetterFunc func;
            TypeCaller(Foam::dictionary &d, GetterFunc f) : dict(d), func(f) {}

            nb::object operator()(const std::string &key, nb::object default_value) const
            {
                if (!dict.found(key))
                {
                    return default_value;
                }
                return func(dict, key);
            }
        };
        return nb::cpp_function(TypeCaller(dict, it->second),
            nb::arg("key"), nb::arg("default_value").none());
    }
};

void bindDict(nanobind::module_ &m)
{
    namespace nb = nanobind;

    nb::class_<Foam::keyType>(m, "keyType")
        .def(nb::init<const Foam::word &>())
        .def("__init__", [](Foam::keyType *self, const std::string &str)
             { new (self) Foam::keyType(Foam::word(str)); });

    nb::class_<Foam::entry>(m, "entry");

    nb::class_<Foam::dictionary>(m, "dictionary")
        .def(nb::init<const std::string &>())
        .def_static("read", [](const std::string &filename) -> Foam::dictionary *
                    {
            // Allocate on heap and return pointer
            return new Foam::dictionary(Foam::read_dictionary(filename)); }, nb::rv_policy::take_ownership)
        .def(nb::init<>())
        .def(nb::init<const Foam::dictionary &>())
        .def("toc", &Foam::dictionary::toc)
        .def("clear", &Foam::dictionary::clear)
        .def("found", [](const Foam::dictionary &self, const std::string key)
             { return self.found(Foam::word(key)); })
        .def("isDict", [](const Foam::dictionary &self, const std::string key)
             { return self.isDict(Foam::word(key)); })
        .def("subDict", [](Foam::dictionary &self, const std::string key)
             { return static_cast<Foam::dictionary *>(&self.subDict(Foam::word(key))); }, nb::rv_policy::reference_internal)
        .def("subDictOrAdd", [](Foam::dictionary &self, const std::string key)
             { return self.subDictOrAdd(Foam::word(key)); }, nb::rv_policy::reference_internal)
        .def("write", [](const Foam::dictionary &self, const std::string file_name)
             {
            Foam::fileName dictFileName(file_name);
            Foam::OFstream os(dictFileName);
            Foam::IOobject::writeBanner(os);
            Foam::IOobject::writeDivider(os);
            self.write(os,false);
            Foam::IOobject::writeEndDivider(os); })
        .def("print", [](const Foam::dictionary &self)
             {
            Foam::IOobject::writeBanner(Foam::Info);
            Foam::IOobject::writeDivider(Foam::Info);
            self.write(Foam::Info,false);
            Foam::IOobject::writeEndDivider(Foam::Info); })
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

        .def("add", [](Foam::dictionary &self, const Foam::entry &e, bool merge)
             { self.add(e, merge); })
        .def("add", [](Foam::dictionary &self, const std::string &key, const Foam::word& value)
             { Foam::add<Foam::word>(self, key, value); }, nb::arg("key"), nb::arg("value"))
        .def("add", &Foam::add<Foam::scalar>)
        .def("add", &Foam::add<Foam::vector>)
        .def("add", &Foam::add<Foam::tensor>)
        .def("add", &Foam::add<Foam::List<Foam::word>>)
        .def("add", &Foam::add<Foam::Field<Foam::scalar>>)
        .def("add", &Foam::add<Foam::Field<Foam::vector>>)
        .def("add", &Foam::add<Foam::Field<Foam::tensor>>)
        .def("lookupSolverPerformanceScalarList", &Foam::lookupSolverPerformanceList<Foam::scalar>)
        .def("lookupSolverPerformanceVectorList", &Foam::lookupSolverPerformanceList<Foam::vector>)
        .def("lookupSolverPerformanceTensorList", &Foam::lookupSolverPerformanceList<Foam::tensor>)
        .def_prop_ro("get", [](Foam::dictionary &self)
                               { return DictionaryGetProxy(self); }, nb::rv_policy::reference_internal)
        .def_prop_ro("getOrDefault", [](Foam::dictionary &self)
                               { return DictionaryGetOrDefaultProxy(self); }, nb::rv_policy::reference_internal);

    nb::class_<DictionaryGetProxy>(m, "DictionaryGetProxy")
        .def("__getitem__", &DictionaryGetProxy::operator[]);

    nb::class_<DictionaryGetOrDefaultProxy>(m, "DictionaryGetOrDefaultProxy")
        .def("__getitem__", &DictionaryGetOrDefaultProxy::operator[]);

    // Register types for get directly
    DictionaryGetProxy::type_registry()["str"] = [](Foam::dictionary &d, const std::string &key)
    {
        auto *entry = d.findEntry(Foam::word(key));
        if (entry->isStream())
        {
            return nb::cast(entry->stream().toString());
        }
        return nb::cast(static_cast<std::string>(d.get<Foam::word>(Foam::word(key))));
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
