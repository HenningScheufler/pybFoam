# sampling module CMakeLists.txt

set(SAMPLING_SOURCES
    bind_sampling.cpp
    sampling.cpp
)

set(SAMPLING_HEADERS
    bind_sampling.hpp
)

# Create the nanobind module
nanobind_add_module(sampling_bindings ${SAMPLING_SOURCES})

# Set target properties
set_target_properties(sampling_bindings PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
)

# Link OpenFOAM libraries - order matters!
target_link_libraries(sampling_bindings PRIVATE
    OpenFOAM::finiteVolume
    OpenFOAM::meshTools
    OpenFOAM::core
)

# Explicitly link the sampling library
target_link_directories(sampling_bindings PRIVATE "${FOAM_LIBBIN}")
target_link_libraries(sampling_bindings PRIVATE -lsampling)

# Add include directories specific to this module
target_include_directories(sampling_bindings PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    "${FOAM_SRC}/sampling/lnInclude"
)

# Install the module
if(DEFINED SKBUILD)
    # scikit-build-core manages the installation
    install(TARGETS sampling_bindings
        LIBRARY DESTINATION pybFoam
        COMPONENT python
    )
else()
    # Standalone build - install to Python site-packages
    install(TARGETS sampling_bindings
        LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
    )
endif()
