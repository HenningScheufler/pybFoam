# vof module CMakeLists.txt
# Provides VoF (Volume of Fluid) bindings for two-phase flow simulations.

set(VOF_SOURCES
    bind_mixture.cpp
    bind_twoPhaseTransport.cpp
    bind_alphaEqn.cpp
    vof.cpp
)

set(VOF_HEADERS
    bind_mixture.hpp
    bind_twoPhaseTransport.hpp
    bind_alphaEqn.hpp
)

# Create the nanobind module (NB_SHARED so it shares the canonical
# libnanobind.so and the cross-module type registry with pybFoam_core).
nanobind_add_module(vof NB_SHARED ${VOF_SOURCES})

# Set target properties
set_target_properties(vof PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
    INSTALL_RPATH "${PYBFOAM_MODULE_INSTALL_RPATH}"
    BUILD_WITH_INSTALL_RPATH ON
)

# Link OpenFOAM VoF libraries (includes mixture, interfaceProperties, turbulence)
target_link_libraries(vof PRIVATE
    OpenFOAM::vof
    OpenFOAM::turbulence
    OpenFOAM::finiteVolume
    OpenFOAM::meshTools
)

# Add include directories specific to this module
target_include_directories(vof PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install the module
if(DEFINED SKBUILD)
    # scikit-build-core manages the installation
    install(TARGETS vof
        LIBRARY DESTINATION pybFoam
        COMPONENT python
    )
else()
    # Standalone build - install to Python site-packages
    install(TARGETS vof
        LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
    )
endif()
