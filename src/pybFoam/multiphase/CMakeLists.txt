# multiphase module CMakeLists.txt
# Provides multiphase (VoF / Volume of Fluid) bindings for two-phase flow simulations.

set(MULTIPHASE_SOURCES
    bind_mixture.cpp
    bind_twoPhaseTransport.cpp
    bind_isoAdvection.cpp
    bind_reconstruction.cpp
    multiphase.cpp
)

set(MULTIPHASE_HEADERS
    bind_mixture.hpp
    bind_twoPhaseTransport.hpp
    bind_isoAdvection.hpp
    bind_reconstruction.hpp
)

# Create the nanobind module (NB_SHARED so it shares the canonical
# libnanobind.so and the cross-module type registry with pybFoam_core).
nanobind_add_module(multiphase NB_SHARED ${MULTIPHASE_SOURCES})

# Set target properties
set_target_properties(multiphase PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
    INSTALL_RPATH "${PYBFOAM_MODULE_INSTALL_RPATH}"
    BUILD_WITH_INSTALL_RPATH ON
)

# Link OpenFOAM VoF libraries (includes mixture, interfaceProperties, turbulence)
target_link_libraries(multiphase PRIVATE
    OpenFOAM::vof
    OpenFOAM::turbulence
    OpenFOAM::finiteVolume
    OpenFOAM::meshTools
)

# Add include directories specific to this module
target_include_directories(multiphase PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install the module
if(DEFINED SKBUILD)
    # scikit-build-core manages the installation
    install(TARGETS multiphase
        LIBRARY DESTINATION pybFoam
        COMPONENT python
    )
else()
    # Standalone build - install to Python site-packages
    install(TARGETS multiphase
        LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
    )
endif()
