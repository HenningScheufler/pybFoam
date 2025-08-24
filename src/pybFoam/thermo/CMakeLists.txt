# thermo module CMakeLists.txt

set(THERMO_SOURCES
    bind_thermo.cpp
    thermo.cpp
)

set(THERMO_HEADERS
    bind_thermo.hpp
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
    OpenFOAM::thermo
    OpenFOAM::finiteVolume
    OpenFOAM::transport
)

# Add include directories specific to this module
target_include_directories(thermo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install the module
if(DEFINED SKBUILD)
    # scikit-build-core manages the installation
    install(TARGETS thermo
        LIBRARY DESTINATION pybFoam
        COMPONENT python
    )
else()
    # Standalone build - install to Python site-packages
    install(TARGETS thermo
        LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
    )
endif()
