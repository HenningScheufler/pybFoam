# mesh_generation module CMakeLists.txt

# CheckMesh utility sources from OpenFOAM
set(CHECKMESH_DIR "$ENV{WM_PROJECT_DIR}/applications/utilities/mesh/manipulation/checkMesh")
set(CHECKMESH_SOURCES
    ${CHECKMESH_DIR}/checkTools.C
    ${CHECKMESH_DIR}/checkTopology.C
    ${CHECKMESH_DIR}/checkGeometry.C
    ${CHECKMESH_DIR}/checkMeshQuality.C
    ${CHECKMESH_DIR}/writeFields.C
)

set(MESH_GENERATION_SOURCES
    bind_blockmesh.cpp
    mesh_utils.C
    mesh_generation.C
    bind_checkmesh.cpp
    bind_snappy.cpp
    ${CHECKMESH_SOURCES}
)

set(MESH_GENERATION_HEADERS
    bind_blockmesh.hpp
    bind_checkmesh.hpp
    bind_snappy.hpp
    mesh_utils.H
)

# Create the pybind11 module
pybind11_add_module(bind_checkmesh ${MESH_GENERATION_SOURCES})

# Set target properties
set_target_properties(bind_checkmesh PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
)

# Link OpenFOAM libraries
target_link_libraries(bind_checkmesh PRIVATE
    OpenFOAM::api
    OpenFOAM::finiteVolume
    OpenFOAM::meshTools
)

# Link blockMesh library directly
find_library(BLOCKMESH_LIB blockMesh HINTS "$ENV{FOAM_LIBBIN}")
if(BLOCKMESH_LIB)
    target_link_libraries(bind_checkmesh PRIVATE ${BLOCKMESH_LIB})
else()
    message(FATAL_ERROR "blockMesh library not found")
endif()

# Link snappyHexMesh library
find_library(SNAPPY_LIB snappyHexMesh HINTS "$ENV{FOAM_LIBBIN}")
if(SNAPPY_LIB)
    target_link_libraries(bind_checkmesh PRIVATE ${SNAPPY_LIB})
else()
    message(FATAL_ERROR "snappyHexMesh library not found")
endif()

# Link dynamicMesh library
find_library(DYNAMIC_MESH_LIB dynamicMesh HINTS "$ENV{FOAM_LIBBIN}")
if(DYNAMIC_MESH_LIB)
    target_link_libraries(bind_checkmesh PRIVATE ${DYNAMIC_MESH_LIB})
else()
    message(FATAL_ERROR "dynamicMesh library not found")
endif()

# Link decompositionMethods library
find_library(DECOMPOSITION_METHODS_LIB decompositionMethods HINTS "$ENV{FOAM_LIBBIN}")
if(DECOMPOSITION_METHODS_LIB)
    target_link_libraries(bind_checkmesh PRIVATE ${DECOMPOSITION_METHODS_LIB})
else()
    message(FATAL_ERROR "decompositionMethods library not found")
endif()

# Add include directories specific to this module
target_include_directories(bind_checkmesh PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    $ENV{WM_PROJECT_DIR}/applications/utilities/mesh/manipulation/checkMesh
)

# Add snappyHexMesh and decomposition include paths
target_include_directories(bind_checkmesh SYSTEM PRIVATE
    ${FOAM_SRC}/mesh/snappyHexMesh/lnInclude
    ${FOAM_SRC}/parallel/decompose/lnInclude
    ${FOAM_SRC}/parallel/decompose/decompositionMethods/lnInclude
)

# Add blockMesh include path as SYSTEM to avoid warnings
target_include_directories(bind_checkmesh SYSTEM PRIVATE
    ${FOAM_SRC}/mesh/blockMesh/lnInclude
)

# Install the module
if(DEFINED SKBUILD)
    # scikit-build-core manages the installation
    install(TARGETS bind_checkmesh
        LIBRARY DESTINATION pybFoam
        COMPONENT python
    )
else()
    # Standalone build - install to Python site-packages
    install(TARGETS bind_checkmesh
        LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
    )
endif()
