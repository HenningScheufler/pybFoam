# Smoke test: verify a downstream OpenFOAM C++ project can find and link
# against pybFoamEmbed via the installed package config.
#
# Usage (after `pip install .` of pybFoam):
#   cmake -B build && cmake --build build

cmake_minimum_required(VERSION 3.18)
project(pybFoamEmbedSmoke CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Reuse pybFoam's FindOpenFOAM.cmake (transitive dep of pybFoamEmbed)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")

find_package(Python COMPONENTS Interpreter Development.Embed REQUIRED)
find_package(OpenFOAM REQUIRED)

# Discover pybFoamEmbed from the installed Python package
list(APPEND CMAKE_PREFIX_PATH "${Python_SITELIB}/pybFoam/embed/cmake")
find_package(pybFoamEmbed REQUIRED)

add_compile_options(-Wno-old-style-cast)

add_library(smoke SHARED smoke.cpp)
target_link_libraries(smoke PUBLIC pybFoamEmbed::pybFoamEmbed)
