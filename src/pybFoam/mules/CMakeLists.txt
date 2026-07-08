# mules module CMakeLists.txt
# Generic MULES bounded-transport limiter bindings.

set(MULES_SOURCES
    bind_mules.cpp
    mules.cpp
)

set(MULES_HEADERS
    bind_mules.hpp
)

# Create the nanobind module (NB_SHARED so it shares the canonical
# libnanobind.so and the cross-module type registry with pybFoam_core).
nanobind_add_module(mules NB_SHARED ${MULES_SOURCES})

# Set target properties
set_target_properties(mules PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
    INSTALL_RPATH "${PYBFOAM_MODULE_INSTALL_RPATH}"
    BUILD_WITH_INSTALL_RPATH ON
)

# Link OpenFOAM libraries (MULES lives in finiteVolume)
target_link_libraries(mules PRIVATE
    OpenFOAM::finiteVolume
)

# Add include directories specific to this module
target_include_directories(mules PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install the module
if(DEFINED SKBUILD)
    # scikit-build-core manages the installation
    install(TARGETS mules
        LIBRARY DESTINATION pybFoam
        COMPONENT python
    )
else()
    # Standalone build - install to Python site-packages
    install(TARGETS mules
        LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
    )
endif()
