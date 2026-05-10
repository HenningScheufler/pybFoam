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

Description
    Helpers that collapse the (T, tmp<T>) overload explosion when binding
    OpenFOAM **free functions** (those registered via `m.def(...)`) to
    Python. Each helper takes a generic lambda whose body defers overload
    resolution to the call site.

    Scope: free functions only — class-method binding (operator overloads on
    nb::class_<>) is left as inline `.def()` chains for clarity.

\*---------------------------------------------------------------------------*/

#ifndef pybfoam_bind_helpers_H
#define pybfoam_bind_helpers_H

#include <nanobind/nanobind.h>
#include "tmp.H"

namespace Foam
{
namespace pybind_helpers
{

namespace nb = nanobind;

template<class FieldType, class Op>
void bindUnary(nb::module_& m, const char* name, Op op)
{
    m.def(name, [op](const FieldType& f){ return op(f); });
    m.def(name, [op](const tmp<FieldType>& f){ return op(f); });
}

template<class... FieldTypes, class Op>
void bindUnaryFor(nb::module_& m, const char* name, Op op)
{
    (bindUnary<FieldTypes>(m, name, op), ...);
}

template<class T1, class T2, class Op>
void bindBinary(nb::module_& m, const char* name, Op op)
{
    m.def(name, [op](const T1& a,      const T2& b)      { return op(a, b); });
    m.def(name, [op](const T1& a,      const tmp<T2>& b) { return op(a, b); });
    m.def(name, [op](const tmp<T1>& a, const T2& b)      { return op(a, b); });
    m.def(name, [op](const tmp<T1>& a, const tmp<T2>& b) { return op(a, b); });
}

}  // namespace pybind_helpers
}  // namespace Foam

#endif
