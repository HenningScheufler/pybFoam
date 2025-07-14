# pybFoam_core module CMakeLists.txt

set(PYBFOAM_CORE_SOURCES
    foam_dict.C
    foam_mesh.C
    foam_primitives.C
    foam_fields.C
    foam_geo_fields.C
    pybFoam.C
)

set(PYBFOAM_CORE_HEADERS
    foam_dict.H
    foam_mesh.H
    foam_primitives.H
    foam_fields.H
    foam_geo_fields.H
    foam_fvc.H
)

# Create the pybind11 module
pybind11_add_module(pybFoam_core ${PYBFOAM_CORE_SOURCES})

# Set target properties
set_target_properties(pybFoam_core PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
)

# Link OpenFOAM libraries
target_link_libraries(pybFoam_core PRIVATE
    ${OPENFOAM_LIBRARIES}
)

# Add include directories specific to this module
target_include_directories(pybFoam_core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENFOAM_INCLUDE_DIRS}
)

# Install the module
install(TARGETS pybFoam_core
    LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
)
