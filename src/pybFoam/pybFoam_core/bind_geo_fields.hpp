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
    Foam::pyInterp

Description

Author
    Henning Scheufler, all rights reserved.

SourceFiles


\*---------------------------------------------------------------------------*/

#ifndef foam_geo_fields
#define foam_geo_fields

// System includes
#include <pybind11/pybind11.h>

#include "bind_mesh.hpp"
#include "GeometricField.H"
#include "volFields.H"
#include "surfaceFields.H"
#include <vector>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

namespace Foam
{
    
template<typename Type>
pybind11::array_t<scalar> FieldToNumpy(const Field<Type>& values);


template< >
pybind11::array_t<scalar> FieldToNumpy<scalar>(const Field<scalar>& values);


template<typename Type>
void NumpyToField(Field<Type>& values,const pybind11::array_t<scalar> np_arr);


template<>
void NumpyToField<scalar>(Field<scalar>& values,const pybind11::array_t<scalar> np_arr);


template<class Type, template<class> class PatchField, class GeoMesh>
const Field<Type>& field
(
    const GeometricField<Type, PatchField, GeoMesh>& vf,
    const fvMesh& mesh, 
    const std::string& name
);


template<class Type, template<class> class PatchField, class GeoMesh>
void field
(
    GeometricField<Type, PatchField, GeoMesh>& vf,
    const fvMesh& mesh,
    const std::string& name,const Field<Type>& f
);


template<class Type, template<class> class PatchField, class GeoMesh>
GeometricField<Type, PatchField, GeoMesh> read_geoField
(
    const std::string& name,
    const fvMesh& mesh
);


template<class Type, template<class> class PatchField, class GeoMesh>
auto
declare_geofields(py::module &m, std::string className);

template<class Type, template<class> class PatchField, class GeoMesh>
GeometricField<scalar, PatchField, GeoMesh>
declare_mag(const GeometricField<Type, PatchField, GeoMesh>& geof);



void  bindGeoFields(pybind11::module& m);

}

#endif // foam_geo_fields  defined 
