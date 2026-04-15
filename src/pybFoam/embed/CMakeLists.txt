# pybFoam embed library - Python interpreter for OpenFOAM embedding
#
# Usage:
#   list(APPEND CMAKE_PREFIX_PATH "$ENV{FOAM_USER_LIBBIN}/cmake")
#   find_package(pybFoamEmbed REQUIRED)
#   target_link_libraries(mylib PUBLIC pybFoamEmbed::pybFoamEmbed)
#
# Installs outside CMAKE_INSTALL_PREFIX into the OpenFOAM user tree:
#   $FOAM_USER_LIBBIN/libpybFoamEmbed.so, libnanobind.so
#   $FOAM_USER_LIBBIN/cmake/pybFoamEmbed/*.cmake
#   $FOAM_USER_LIBBIN/../include/pybFoamEmbed/pyInterp.hpp
#   $FOAM_USER_LIBBIN/../include/pybFoamEmbed/nanobind/include/...

include(CMakePackageConfigHelpers)

# Install layout — all paths derived from FOAM_USER_LIBBIN
set(PYBFOAM_EMBED_INSTALL_LIBDIR     "$ENV{FOAM_USER_LIBBIN}")
set(PYBFOAM_EMBED_INSTALL_INCLUDEDIR "$ENV{FOAM_USER_LIBBIN}/../include")
set(PYBFOAM_EMBED_INSTALL_CMAKEDIR   "${PYBFOAM_EMBED_INSTALL_LIBDIR}/cmake/pybFoamEmbed")

# --- nanobind (shared lib for single type registry across all modules) -----

# Prefixed target name avoids collisions with downstream nanobind usage
nanobind_build_library(pybFoamEmbed_nanobind)
add_library(pybFoamEmbed::nanobind ALIAS pybFoamEmbed_nanobind)

set_target_properties(pybFoamEmbed_nanobind PROPERTIES
    OUTPUT_NAME nanobind
    EXPORT_NAME nanobind)

# Replace nanobind's build-time include paths with vendored header location
set_target_properties(pybFoamEmbed_nanobind PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "")
target_include_directories(pybFoamEmbed_nanobind INTERFACE
    $<BUILD_INTERFACE:${NB_DIR}/include>
    $<INSTALL_INTERFACE:${PYBFOAM_EMBED_INSTALL_INCLUDEDIR}/pybFoamEmbed/nanobind/include>)

# --- pybFoamEmbed -----------------------------------------------------------

add_library(pybFoamEmbed SHARED pyInterp.cpp)
add_library(pybFoamEmbed::pybFoamEmbed ALIAS pybFoamEmbed)

set_target_properties(pybFoamEmbed PROPERTIES
    OUTPUT_NAME pybFoamEmbed)

target_link_libraries(pybFoamEmbed PUBLIC
    OpenFOAM::finiteVolume
    Python::Python
    pybFoamEmbed::nanobind)

target_compile_definitions(pybFoamEmbed PUBLIC NB_SHARED)

target_include_directories(pybFoamEmbed PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${PYBFOAM_EMBED_INSTALL_INCLUDEDIR}/pybFoamEmbed>)

# --- Install ----------------------------------------------------------------

install(TARGETS pybFoamEmbed pybFoamEmbed_nanobind
    EXPORT  pybFoamEmbedTargets
    LIBRARY DESTINATION ${PYBFOAM_EMBED_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${PYBFOAM_EMBED_INSTALL_LIBDIR})

install(FILES pyInterp.hpp
    DESTINATION ${PYBFOAM_EMBED_INSTALL_INCLUDEDIR}/pybFoamEmbed)

# Vendor nanobind headers so consumers don't need the nanobind Python package
install(DIRECTORY ${NB_DIR}/include/
    DESTINATION ${PYBFOAM_EMBED_INSTALL_INCLUDEDIR}/pybFoamEmbed/nanobind/include)

install(EXPORT pybFoamEmbedTargets
    FILE      pybFoamEmbedTargets.cmake
    NAMESPACE pybFoamEmbed::
    DESTINATION ${PYBFOAM_EMBED_INSTALL_CMAKEDIR})

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/pybFoamEmbedConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/pybFoamEmbedConfig.cmake
    INSTALL_DESTINATION ${PYBFOAM_EMBED_INSTALL_CMAKEDIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pybFoamEmbedConfig.cmake
    DESTINATION ${PYBFOAM_EMBED_INSTALL_CMAKEDIR})
