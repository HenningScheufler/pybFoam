cmake_minimum_required(VERSION 3.18)

# Set CMake policies for compatibility
if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

project(pybFoam VERSION 0.1.9 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Configure dependencies with CPM
include(Dependencies)

# Find Python (scikit-build-core sets up Python variables)
if(NOT Python_FOUND)
    find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
endif()

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

# ===================================================================
# Stub generation for type hints
# ===================================================================
# Generate Python type stubs (.pyi files) using pybind11-stubgen
# Stubs are generated after all modules build, then cleaned and copied
# Note: pybind11-stubgen must be installed in the Python environment

option(
    ENABLE_PYBFOAM_STUBS
    "Generate stub files and copy to source directory for development"
    OFF)

message(STATUS "Stub generation for pybFoam: ${ENABLE_PYBFOAM_STUBS}")

# Set variables that subdirectories can use
if(${ENABLE_PYBFOAM_STUBS})
    # Determine the module directory and stub output directory
    if(DEFINED SKBUILD)
        set(MODULE_DIR "${CMAKE_CURRENT_BINARY_DIR}/src/pybFoam" PARENT_SCOPE)
    else()
        set(MODULE_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" PARENT_SCOPE)
    endif()
    
    set(STUB_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/stubs" PARENT_SCOPE)
    set(STUB_TARGET_DIR "${PROJECT_SOURCE_DIR}/src/pybFoam" PARENT_SCOPE)
    
    message(STATUS "Stub output directory: ${CMAKE_CURRENT_BINARY_DIR}/stubs")
    message(STATUS "Stub target directory: ${PROJECT_SOURCE_DIR}/src/pybFoam")
endif()

# Add subdirectories
add_subdirectory(src/pybFoam)

# Add stub generation after all targets are created
if(${ENABLE_PYBFOAM_STUBS})
    # Clean and copy stubs after all modules are built
    add_custom_target(generate_and_clean_stubs ALL
        # Run pybind11-stubgen for each top-level module
        COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/src/pybFoam"
                ${Python_EXECUTABLE} -m pybind11_stubgen
                --output-dir "${CMAKE_CURRENT_BINARY_DIR}/stubs"
                --print-invalid-expressions-as-is
                pybFoam_core
        COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/src/pybFoam"
                ${Python_EXECUTABLE} -m pybind11_stubgen
                --output-dir "${CMAKE_CURRENT_BINARY_DIR}/stubs"
                --print-invalid-expressions-as-is
                fvc
        COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/src/pybFoam"
                ${Python_EXECUTABLE} -m pybind11_stubgen
                --output-dir "${CMAKE_CURRENT_BINARY_DIR}/stubs"
                --print-invalid-expressions-as-is
                fvm
        COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/src/pybFoam"
                ${Python_EXECUTABLE} -m pybind11_stubgen
                --output-dir "${CMAKE_CURRENT_BINARY_DIR}/stubs"
                --print-invalid-expressions-as-is
                turbulence
        COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/src/pybFoam"
                ${Python_EXECUTABLE} -m pybind11_stubgen
                --output-dir "${CMAKE_CURRENT_BINARY_DIR}/stubs"
                --print-invalid-expressions-as-is
                thermo
        COMMAND ${CMAKE_COMMAND} -E env "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/src/pybFoam"
                ${Python_EXECUTABLE} -m pybind11_stubgen
                --output-dir "${CMAKE_CURRENT_BINARY_DIR}/stubs"
                --print-invalid-expressions-as-is
                runTimeTables
        # Clean stubs
        COMMAND ${Python_EXECUTABLE}
                ${PROJECT_SOURCE_DIR}/scripts/clean_stubs.py
        # Copy to source directory
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_BINARY_DIR}/stubs"
                "${PROJECT_SOURCE_DIR}/src/pybFoam"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating, cleaning, and copying stub files"
        DEPENDS pybFoam_core fvc fvm turbulence thermo runTimeTables _sampling
        VERBATIM
    )
endif()

# Optional: Add applications and tests only if not building with pip
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    # add_subdirectory(src/embeddingPython)
    # add_subdirectory(application EXCLUDE_FROM_ALL)
    # enable_testing()
    # add_subdirectory(testsuite EXCLUDE_FROM_ALL)
endif()
