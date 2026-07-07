# pybFoam embed library — Python interpreter wrapper for OpenFOAM embedding.
#
# Links against the venv's libpython3.X.so and OpenFOAM's finiteVolume.
# Does NOT depend on nanobind: pyInterp.{hpp,cpp} only call Py_* APIs.
# Ships inside the pybFoam wheel under site-packages/pybFoam/embed/ with
# its CMake config + headers; consumers (OpenFOAM solvers, function objects)
# discover it via:
#
#   list(APPEND CMAKE_PREFIX_PATH "${Python_SITELIB}/pybFoam/embed/cmake")
#   find_package(pybFoamEmbed REQUIRED)
#   target_link_libraries(mylib PUBLIC pybFoamEmbed::pybFoamEmbed)
#
# The shared libnanobind.so used by pybFoam's Python binding modules is a
# private implementation detail of those modules and lives in pybFoam/, not
# under embed/. See ../CMakeLists.txt.

include(CMakePackageConfigHelpers)

# Install layout — relative paths inside the wheel
set(PYBFOAM_EMBED_INSTALL_LIBDIR     "pybFoam/embed/lib")
set(PYBFOAM_EMBED_INSTALL_INCLUDEDIR "pybFoam/embed/include")
set(PYBFOAM_EMBED_INSTALL_CMAKEDIR   "pybFoam/embed/cmake/pybFoamEmbed")

# --- pybFoamEmbed -----------------------------------------------------------

add_library(pybFoamEmbed SHARED pyInterp.cpp)
add_library(pybFoamEmbed::pybFoamEmbed ALIAS pybFoamEmbed)

set_target_properties(pybFoamEmbed PROPERTIES
    OUTPUT_NAME pybFoamEmbed)

target_link_libraries(pybFoamEmbed PUBLIC
    OpenFOAM::finiteVolume
    Python::Python)

target_include_directories(pybFoamEmbed PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${PYBFOAM_EMBED_INSTALL_INCLUDEDIR}/pybFoamEmbed>)

# --- CMake package config ---------------------------------------------------

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/pybFoamEmbedConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/pybFoamEmbedConfig.cmake
    INSTALL_DESTINATION ${PYBFOAM_EMBED_INSTALL_CMAKEDIR})

# --- Install ---------------------------------------------------------------

install(TARGETS pybFoamEmbed
    EXPORT  pybFoamEmbedTargets
    LIBRARY DESTINATION ${PYBFOAM_EMBED_INSTALL_LIBDIR})

install(FILES pyInterp.hpp
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
