# pybFoamEmbed: header-only package for embedding Python in OpenFOAM.
#
# Nothing is compiled here. pyInterp.hpp and nanobindEmbed.hpp are compiled into
# each consumer, because nanobind's state pointer is hidden per shared library.
# A consumer that uses nanobind links its own static nanobind and shares types
# with pybFoam through nanobind's ABI key.
# Ships inside the pybFoam wheel under site-packages/pybFoam/embed/ with its
# CMake config and headers; consumers (OpenFOAM solvers, function objects)
# discover it via:
#
#   list(APPEND CMAKE_PREFIX_PATH "${Python_SITELIB}/pybFoam/embed/cmake")
#   find_package(pybFoamEmbed REQUIRED)
#   target_link_libraries(mylib PUBLIC pybFoamEmbed::pybFoamEmbed)

include(CMakePackageConfigHelpers)

# Install layout: relative paths inside the wheel
set(PYBFOAM_EMBED_INSTALL_INCLUDEDIR "pybFoam/embed/include")
set(PYBFOAM_EMBED_INSTALL_CMAKEDIR   "pybFoam/embed/cmake/pybFoamEmbed")

# --- pybFoamEmbed -----------------------------------------------------------

add_library(pybFoamEmbed INTERFACE)
add_library(pybFoamEmbed::pybFoamEmbed ALIAS pybFoamEmbed)

target_link_libraries(pybFoamEmbed INTERFACE
    OpenFOAM::finiteVolume
    Python::Python
    ${CMAKE_DL_LIBS})

target_include_directories(pybFoamEmbed INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${PYBFOAM_EMBED_INSTALL_INCLUDEDIR}/pybFoamEmbed>)

# --- CMake package config ---------------------------------------------------

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/pybFoamEmbedConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/pybFoamEmbedConfig.cmake
    INSTALL_DESTINATION ${PYBFOAM_EMBED_INSTALL_CMAKEDIR})

# --- Install ---------------------------------------------------------------

install(TARGETS pybFoamEmbed
    EXPORT  pybFoamEmbedTargets)

install(FILES pyInterp.hpp nanobindEmbed.hpp
    DESTINATION ${PYBFOAM_EMBED_INSTALL_INCLUDEDIR}/pybFoamEmbed)

# Vendor nanobind headers so OpenFOAM-side consumers that want to manipulate
# nanobind objects from C++ don't need a separate `pip install nanobind`.
install(DIRECTORY ${NB_DIR}/include/
    DESTINATION ${PYBFOAM_EMBED_INSTALL_INCLUDEDIR}/pybFoamEmbed/nanobind/include)

install(EXPORT pybFoamEmbedTargets
    FILE      pybFoamEmbedTargets.cmake
    NAMESPACE pybFoamEmbed::
    DESTINATION ${PYBFOAM_EMBED_INSTALL_CMAKEDIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pybFoamEmbedConfig.cmake
    DESTINATION ${PYBFOAM_EMBED_INSTALL_CMAKEDIR})
