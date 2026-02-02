# mesh_generation module CMakeLists.txt

# Add checkMesh utility sources
set(CHECKMESH_DIR "$ENV{WM_PROJECT_DIR}/applications/utilities/mesh/manipulation/checkMesh")

set(MESHING_SOURCES
    bind_blockmesh.cpp
    mesh_utils.C
    mesh_generation.C
    bind_checkmesh.cpp
    bind_snappy.cpp
    ${CHECKMESH_DIR}/checkGeometry.C
    ${CHECKMESH_DIR}/checkTopology.C
    ${CHECKMESH_DIR}/checkTools.C
    ${CHECKMESH_DIR}/writeFields.C
    ${CHECKMESH_DIR}/checkMeshQuality.C
)

set(MESHING_HEADERS
    bind_blockmesh.hpp
    bind_checkmesh.hpp
    bind_snappy.hpp
    mesh_utils.H
)

# Create the pybind11 module
pybind11_add_module(meshing ${MESHING_SOURCES})

# Set target properties
set_target_properties(meshing PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
)

# Link OpenFOAM libraries
target_link_libraries(meshing PRIVATE
    OpenFOAM::api
    OpenFOAM::finiteVolume
    OpenFOAM::meshTools
)

# Link blockMesh library directly
find_library(BLOCKMESH_LIB blockMesh HINTS "$ENV{FOAM_LIBBIN}")
if(BLOCKMESH_LIB)
    target_link_libraries(meshing PRIVATE ${BLOCKMESH_LIB})
else()
    message(FATAL_ERROR "blockMesh library not found")
endif()

# Link snappyHexMesh library
find_library(SNAPPY_LIB snappyHexMesh HINTS "$ENV{FOAM_LIBBIN}")
if(SNAPPY_LIB)
    target_link_libraries(meshing PRIVATE ${SNAPPY_LIB})
else()
    message(FATAL_ERROR "snappyHexMesh library not found")
endif()

# Link dynamicMesh library
find_library(DYNAMIC_MESH_LIB dynamicMesh HINTS "$ENV{FOAM_LIBBIN}")
if(DYNAMIC_MESH_LIB)
    target_link_libraries(meshing PRIVATE ${DYNAMIC_MESH_LIB})
else()
    message(FATAL_ERROR "dynamicMesh library not found")
endif()

# Link decompositionMethods library
find_library(DECOMPOSITION_METHODS_LIB decompositionMethods HINTS "$ENV{FOAM_LIBBIN}")
if(DECOMPOSITION_METHODS_LIB)
    target_link_libraries(meshing PRIVATE ${DECOMPOSITION_METHODS_LIB})
else()
    message(FATAL_ERROR "decompositionMethods library not found")
endif()

# Add include directories specific to this module
target_include_directories(meshing PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    $ENV{WM_PROJECT_DIR}/applications/utilities/mesh/manipulation/checkMesh
)

# Add snappyHexMesh and decomposition include paths
target_include_directories(meshing SYSTEM PRIVATE
    ${FOAM_SRC}/mesh/snappyHexMesh/lnInclude
    ${FOAM_SRC}/parallel/decompose/lnInclude
    ${FOAM_SRC}/parallel/decompose/decompositionMethods/lnInclude
)

# Add blockMesh include path as SYSTEM to avoid warnings
target_include_directories(meshing SYSTEM PRIVATE
    ${FOAM_SRC}/mesh/blockMesh/lnInclude
)

# Install the module
if(DEFINED SKBUILD)
    # scikit-build-core manages the installation
    install(TARGETS meshing
        LIBRARY DESTINATION pybFoam
        COMPONENT python
    )
else()
    # Standalone build - install to Python site-packages
    install(TARGETS meshing
        LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
    )
endif()
