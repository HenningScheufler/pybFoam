# thermo module CMakeLists.txt

set(THERMO_SOURCES
    foam_thermo.C
    thermo.C
)

set(THERMO_HEADERS
    foam_thermo.H
)

# Create the pybind11 module
pybind11_add_module(thermo ${THERMO_SOURCES})

# Set target properties
set_target_properties(thermo PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
)

# Link OpenFOAM libraries
target_link_libraries(thermo PRIVATE
    OpenFOAM::api
)

# Add include directories specific to this module
target_include_directories(thermo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install the module
install(TARGETS thermo
    LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
)
