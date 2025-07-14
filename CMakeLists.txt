cmake_minimum_required(VERSION 3.18)

project(pybFoam VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Configure dependencies with CPM
include(Dependencies)

# Find Python
find_package(Python COMPONENTS Interpreter Development REQUIRED)

# Find OpenFOAM
find_package(OpenFOAM REQUIRED)

# Add compiler flags for OpenFOAM compatibility
add_compile_options(-Wno-old-style-cast)

# Include directories
include_directories(${OPENFOAM_INCLUDE_DIRS})
link_directories(${OPENFOAM_LIBRARY_DIRS})

# Add subdirectories
add_subdirectory(src/embeddingPython)
add_subdirectory(src/pybFoam)

# Optional: Add applications
add_subdirectory(application EXCLUDE_FROM_ALL)

# Optional: Add tests
enable_testing()
add_subdirectory(testsuite EXCLUDE_FROM_ALL)
