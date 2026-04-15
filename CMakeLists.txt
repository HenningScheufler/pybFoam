cmake_minimum_required(VERSION 3.18)

# Set CMake policies for compatibility
if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

project(pybFoam VERSION 0.4.1 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

# Build embed library when OpenFOAM is sourced; override with -DPYBFOAM_BUILD_EMBED=ON|OFF
if(DEFINED ENV{FOAM_USER_LIBBIN})
    set(_PYBFOAM_BUILD_EMBED_DEFAULT ON)
else()
    set(_PYBFOAM_BUILD_EMBED_DEFAULT OFF)
endif()
option(PYBFOAM_BUILD_EMBED
    "Build the pybFoamEmbed C++ library for OpenFOAM solvers"
    ${_PYBFOAM_BUILD_EMBED_DEFAULT})

# Development.Embed only needed when building the embed library (links libpython)
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
    message(STATUS "Building pybFoamEmbed (C++ embed library for OpenFOAM)")
    add_subdirectory(src/pybFoam/embed)
endif()
