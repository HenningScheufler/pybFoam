# sampling module CMakeLists.txt

set(SAMPLING_SOURCES
    bind_sampling.cpp
    sampling.cpp
)

set(SAMPLING_HEADERS
    bind_sampling.hpp
)

# Create the pybind11 module
pybind11_add_module(sampling ${SAMPLING_SOURCES})

# Set target properties
set_target_properties(sampling PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
)

# Link OpenFOAM libraries - order matters!
target_link_libraries(sampling PRIVATE
    OpenFOAM::finiteVolume
    OpenFOAM::meshTools
    OpenFOAM::core
)

# Explicitly link the sampling library
target_link_directories(sampling PRIVATE "${FOAM_LIBBIN}")
target_link_libraries(sampling PRIVATE -lsampling)

# Add include directories specific to this module
target_include_directories(sampling PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    "${FOAM_SRC}/sampling/lnInclude"
)

# Install the module
if(DEFINED SKBUILD)
    # scikit-build-core manages the installation
    install(TARGETS sampling
        LIBRARY DESTINATION pybFoam
        COMPONENT python
    )
else()
    # Standalone build - install to Python site-packages
    install(TARGETS sampling
        LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
    )
endif()
