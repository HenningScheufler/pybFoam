# turbulence module CMakeLists.txt

set(TURBULENCE_SOURCES
    bind_turbulence.cpp
    turbulence.cpp
)

set(TURBULENCE_HEADERS
    bind_turbulence.hpp
)

# Create the pybind11 module
pybind11_add_module(turbulence ${TURBULENCE_SOURCES})

# Set target properties
set_target_properties(turbulence PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
)

# Link OpenFOAM libraries
target_link_libraries(turbulence PRIVATE
    OpenFOAM::turbulence
    OpenFOAM::finiteVolume
    OpenFOAM::transport
)

# Add include directories specific to this module
target_include_directories(turbulence PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install the module
if(DEFINED SKBUILD)
    # scikit-build-core manages the installation
    install(TARGETS turbulence
        LIBRARY DESTINATION pybFoam
        COMPONENT python
    )
else()
    # Standalone build - install to Python site-packages
    install(TARGETS turbulence
        LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
    )
endif()
