# pybFoam embed library - Python interpreter for OpenFOAM embedding
#
# Provides the pybFoamEmbed::pybFoamEmbed target for downstream projects.
# Usage:
#   find_package(pybFoamEmbed REQUIRED)
#   target_link_libraries(mylib PUBLIC pybFoamEmbed::pybFoamEmbed)

# Build nanobind as a shared library so downstream consumers
# share the same nanobind internals and type registry
nanobind_build_library(nanobind)

add_library(pybFoamEmbed SHARED pyInterp.cpp)

set_target_properties(pybFoamEmbed PROPERTIES
    OUTPUT_NAME "pybFoamEmbed"
)

# All PUBLIC so downstream targets inherit them
target_link_libraries(pybFoamEmbed PUBLIC
    OpenFOAM::finiteVolume
    Python::Python
    nanobind
)

target_compile_definitions(pybFoamEmbed PUBLIC NB_SHARED)

target_include_directories(pybFoamEmbed PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:$ENV{FOAM_USER_LIBBIN}/../include/pybFoamEmbed>
)

# --- Install ---

set(PYBFOAM_EMBED_INSTALL_LIBDIR "$ENV{FOAM_USER_LIBBIN}")
set(PYBFOAM_EMBED_INSTALL_CMAKEDIR "$ENV{FOAM_USER_LIBBIN}/../lib/cmake/pybFoamEmbed")
set(PYBFOAM_EMBED_INSTALL_INCLUDEDIR "$ENV{FOAM_USER_LIBBIN}/../include")

# Install libraries
install(TARGETS pybFoamEmbed nanobind
    EXPORT pybFoamEmbedTargets
    LIBRARY DESTINATION ${PYBFOAM_EMBED_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${PYBFOAM_EMBED_INSTALL_LIBDIR}
)

# Install header
install(FILES pyInterp.hpp
    DESTINATION ${PYBFOAM_EMBED_INSTALL_INCLUDEDIR}/pybFoamEmbed
)

# Export targets
install(EXPORT pybFoamEmbedTargets
    FILE pybFoamEmbedTargets.cmake
    NAMESPACE pybFoamEmbed::
    DESTINATION ${PYBFOAM_EMBED_INSTALL_CMAKEDIR}
)

# Generate and install config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pybFoamEmbedConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/pybFoamEmbedConfig.cmake
    INSTALL_DESTINATION ${PYBFOAM_EMBED_INSTALL_CMAKEDIR}
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pybFoamEmbedConfig.cmake
    DESTINATION ${PYBFOAM_EMBED_INSTALL_CMAKEDIR}
)
