cmake_minimum_required(VERSION 3.18)

# Set CMake policies for compatibility
if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

# The version lives only in src/pybFoam/_version.py (pyproject.toml reads it too)
set(_pybfoam_version_file "${CMAKE_CURRENT_SOURCE_DIR}/src/pybFoam/_version.py")
file(STRINGS "${_pybfoam_version_file}" _pybfoam_version REGEX "^__version__ *= *\"")
string(REGEX REPLACE "^__version__ *= *\"([^\"]+)\".*" "\\1" _pybfoam_version "${_pybfoam_version}")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_pybfoam_version_file}")

project(pybFoam VERSION ${_pybfoam_version} LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

# The header-only embed package ships inside the pybFoam wheel
# (site-packages/pybFoam/embed/). Disable with -DPYBFOAM_BUILD_EMBED=OFF when it
# is not needed.
option(PYBFOAM_BUILD_EMBED
    "Install the header-only pybFoamEmbed package for OpenFOAM solvers"
    ON)

# Development.Embed only needed for the embed package (consumers link libpython)
set(_pybfoam_python_components Interpreter Development.Module)
if(PYBFOAM_BUILD_EMBED)
    list(APPEND _pybfoam_python_components Development.Embed)
endif()
find_package(Python COMPONENTS ${_pybfoam_python_components} REQUIRED)

# Configure dependencies with CPM (nanobind discovery requires Python)
include(Dependencies)

# Find OpenFOAM
find_package(OpenFOAM REQUIRED)

# Add compiler flags for OpenFOAM compatibility
add_compile_options(-Wno-old-style-cast)

# Set proper output directories
# scikit-build-core sets SKBUILD to indicate it's managing the build
if(DEFINED SKBUILD)
    # Let scikit-build-core handle the install locations
    message(STATUS "Building with scikit-build-core")
else()
    # Standalone build - set our own output directories
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
    set(PYTHON_INSTALL_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
endif()

# Add subdirectories
add_subdirectory(src/pybFoam)

if(PYBFOAM_BUILD_EMBED)
    message(STATUS "Installing pybFoamEmbed (header-only embed package for OpenFOAM)")
    add_subdirectory(src/pybFoam/embed)
endif()
