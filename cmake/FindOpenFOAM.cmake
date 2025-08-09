# CMake module to find OpenFOAM
# This module defines:
#  OPENFOAM_FOUND - System has OpenFOAM
#  OpenFOAM::core - Core OpenFOAM library
#  OpenFOAM::finiteVolume - Finite volume library
#  OpenFOAM::meshTools - Mesh tools library
#  OpenFOAM::thermo - Thermophysical models
#  OpenFOAM::turbulence - Turbulence models
#  OpenFOAM::transport - Transport models
#  OpenFOAM::api - All libraries combined

# ============================================================================
# Check OpenFOAM environment
# ============================================================================
if(NOT DEFINED ENV{FOAM_SRC})
    message(FATAL_ERROR "OpenFOAM environment not sourced. Please source OpenFOAM before running CMake.")
endif()

if(NOT DEFINED ENV{WM_LABEL_SIZE})
    message(FATAL_ERROR "WM_LABEL_SIZE environment variable not set. Please source OpenFOAM environment.")
endif()

if(NOT DEFINED ENV{WM_PRECISION_OPTION})
    message(FATAL_ERROR "WM_PRECISION_OPTION environment variable not set. Please source OpenFOAM environment.")
endif()

if(NOT DEFINED ENV{FOAM_API})
    message(FATAL_ERROR "FOAM_API environment variable not set. Please source OpenFOAM environment.")
endif()

# Set paths
set(FOAM_SRC "$ENV{FOAM_SRC}")
set(FOAM_LIBBIN "$ENV{FOAM_LIBBIN}")

# ============================================================================
# Verify OpenFOAM installation
# ============================================================================
find_path(OPENFOAM_INCLUDE_DIR
    NAMES fvCFD.H
    PATHS "${FOAM_SRC}/finiteVolume/lnInclude"
    NO_DEFAULT_PATH
)

find_library(OPENFOAM_CORE_LIBRARY
    NAMES OpenFOAM
    PATHS "${FOAM_LIBBIN}"
    NO_DEFAULT_PATH
)

if(NOT OPENFOAM_INCLUDE_DIR)
    message(FATAL_ERROR "OpenFOAM headers not found in ${FOAM_SRC}")
endif()

if(NOT OPENFOAM_CORE_LIBRARY)
    message(FATAL_ERROR "OpenFOAM libraries not found in ${FOAM_LIBBIN}")
endif()

# ============================================================================
# Detect OpenFOAM version
# ============================================================================
if(EXISTS "${FOAM_SRC}/OpenFOAM/lnInclude/foamVersion.H")
    file(READ "${FOAM_SRC}/OpenFOAM/lnInclude/foamVersion.H" FOAM_VERSION_FILE)
    string(REGEX MATCH "#define OPENFOAM ([0-9]+)" FOAM_VERSION_MATCH "${FOAM_VERSION_FILE}")
    if(FOAM_VERSION_MATCH)
        set(OPENFOAM_VERSION ${CMAKE_MATCH_1})
    endif()
endif()

# ============================================================================
# Create OpenFOAM::core target
# ============================================================================
if(NOT TARGET OpenFOAM::core)
    add_library(OpenFOAM::core INTERFACE IMPORTED)
    
    target_include_directories(OpenFOAM::core INTERFACE
        "${FOAM_SRC}/OpenFOAM/lnInclude"
        "${FOAM_SRC}/OSspecific/POSIX/lnInclude"
    )
    
    target_link_libraries(OpenFOAM::core INTERFACE OpenFOAM)
    target_link_directories(OpenFOAM::core INTERFACE "${FOAM_LIBBIN}")
    
    target_compile_definitions(OpenFOAM::core INTERFACE 
        WM_LABEL_SIZE=$ENV{WM_LABEL_SIZE} 
        NoRepository
        WM_$ENV{WM_PRECISION_OPTION} 
        OPENFOAM=$ENV{FOAM_API}
    )
endif()

# ============================================================================
# Create OpenFOAM::fileFormats target
# ============================================================================
if(NOT TARGET OpenFOAM::fileFormats)
    add_library(OpenFOAM::fileFormats INTERFACE IMPORTED)
    
    target_include_directories(OpenFOAM::fileFormats INTERFACE
        "${FOAM_SRC}/fileFormats/lnInclude"
        "${FOAM_SRC}/surfMesh/lnInclude"
    )
    
    target_link_libraries(OpenFOAM::fileFormats INTERFACE 
        fileFormats
        OpenFOAM::core
    )
    target_link_directories(OpenFOAM::fileFormats INTERFACE "${FOAM_LIBBIN}")
endif()

# ============================================================================
# Create OpenFOAM::meshTools target
# ============================================================================
if(NOT TARGET OpenFOAM::meshTools)
    add_library(OpenFOAM::meshTools INTERFACE IMPORTED)
    
    target_include_directories(OpenFOAM::meshTools INTERFACE
        "${FOAM_SRC}/meshTools/lnInclude"
        "${FOAM_SRC}/dynamicMesh/lnInclude"
    )
    
    target_link_libraries(OpenFOAM::meshTools INTERFACE 
        meshTools
        surfMesh
        OpenFOAM::core
    )
    target_link_directories(OpenFOAM::meshTools INTERFACE "${FOAM_LIBBIN}")
endif()

# ============================================================================
# Create OpenFOAM::finiteVolume target
# ============================================================================
if(NOT TARGET OpenFOAM::finiteVolume)
    add_library(OpenFOAM::finiteVolume INTERFACE IMPORTED)
    
    target_include_directories(OpenFOAM::finiteVolume INTERFACE
        "${FOAM_SRC}/finiteVolume/lnInclude"
        "${FOAM_SRC}/dynamicFvMesh/lnInclude"
        "${FOAM_SRC}/dynamicMesh/lnInclude"
    )
    
    target_link_libraries(OpenFOAM::finiteVolume INTERFACE 
        finiteVolume
        dynamicMesh
        dynamicFvMesh
        OpenFOAM::meshTools
        OpenFOAM::fileFormats
        OpenFOAM::core
    )
    target_link_directories(OpenFOAM::finiteVolume INTERFACE "${FOAM_LIBBIN}")
endif()

# ============================================================================
# Create OpenFOAM::thermo target
# ============================================================================
if(NOT TARGET OpenFOAM::thermo)
    add_library(OpenFOAM::thermo INTERFACE IMPORTED)
    
    target_include_directories(OpenFOAM::thermo INTERFACE
        "${FOAM_SRC}/thermophysicalModels/basic/lnInclude"
        "${FOAM_SRC}/thermophysicalModels/solidThermo/lnInclude"
    )
    
    target_link_libraries(OpenFOAM::thermo INTERFACE 
        fluidThermophysicalModels
        solidThermo
        specie
        OpenFOAM::core
    )
    target_link_directories(OpenFOAM::thermo INTERFACE "${FOAM_LIBBIN}")
endif()

# ============================================================================
# Create OpenFOAM::turbulence target
# ============================================================================
if(NOT TARGET OpenFOAM::turbulence)
    add_library(OpenFOAM::turbulence INTERFACE IMPORTED)
    
    target_include_directories(OpenFOAM::turbulence INTERFACE
        "${FOAM_SRC}/TurbulenceModels/turbulenceModels/lnInclude"
        "${FOAM_SRC}/TurbulenceModels/incompressible/lnInclude"
        "${FOAM_SRC}/TurbulenceModels/compressible/lnInclude"
    )
    
    target_link_libraries(OpenFOAM::turbulence INTERFACE 
        turbulenceModels
        incompressibleTurbulenceModels
        compressibleTurbulenceModels
        # dynamicFvMesh
        OpenFOAM::core
        OpenFOAM::thermo
    )
    target_link_directories(OpenFOAM::turbulence INTERFACE "${FOAM_LIBBIN}")
endif()

# ============================================================================
# Create OpenFOAM::transport target
# ============================================================================
if(NOT TARGET OpenFOAM::transport)
    add_library(OpenFOAM::transport INTERFACE IMPORTED)
    
    target_include_directories(OpenFOAM::transport INTERFACE
        "${FOAM_SRC}/transportModels"
        "${FOAM_SRC}/transportModels/incompressible/singlePhaseTransportModel"
        "${FOAM_SRC}/transportModels/compressible/lnInclude"
    )
    
    target_link_libraries(OpenFOAM::transport INTERFACE 
        incompressibleTransportModels
        compressibleTransportModels
        OpenFOAM::core
    )
    target_link_directories(OpenFOAM::transport INTERFACE "${FOAM_LIBBIN}")
endif()

# ============================================================================
# Create OpenFOAM::lagrangian target
# ============================================================================
if(NOT TARGET OpenFOAM::lagrangian)
    add_library(OpenFOAM::lagrangian INTERFACE IMPORTED)
    
    target_include_directories(OpenFOAM::lagrangian INTERFACE
        "${FOAM_SRC}/lagrangian/basic/lnInclude"
    )
    
    target_link_libraries(OpenFOAM::lagrangian INTERFACE 
        lagrangian
        OpenFOAM::core
    )
    target_link_directories(OpenFOAM::lagrangian INTERFACE "${FOAM_LIBBIN}")
endif()

# ============================================================================
# Create OpenFOAM::api target (combines all libraries)
# ============================================================================
if(NOT TARGET OpenFOAM::api)
    add_library(OpenFOAM::api INTERFACE IMPORTED)
    
    target_link_libraries(OpenFOAM::api INTERFACE
        OpenFOAM::core
        OpenFOAM::finiteVolume
        OpenFOAM::meshTools
        OpenFOAM::thermo
        OpenFOAM::turbulence
        OpenFOAM::transport
        OpenFOAM::fileFormats
        OpenFOAM::lagrangian
    )
endif()

# ============================================================================
# Legacy compatibility - set traditional CMake variables
# ============================================================================
set(OPENFOAM_FOUND TRUE)
set(OPENFOAM_INCLUDE_DIRS
    "${FOAM_SRC}/OpenFOAM/lnInclude"
    "${FOAM_SRC}/OSspecific/POSIX/lnInclude"
    "${FOAM_SRC}/finiteVolume/lnInclude"
    "${FOAM_SRC}/meshTools/lnInclude"
    "${FOAM_SRC}/dynamicMesh/lnInclude"
    "${FOAM_SRC}/thermophysicalModels/basic/lnInclude"
    "${FOAM_SRC}/thermophysicalModels/solidThermo/lnInclude"
    "${FOAM_SRC}/TurbulenceModels/turbulenceModels/lnInclude"
    "${FOAM_SRC}/TurbulenceModels/incompressible/lnInclude"
    "${FOAM_SRC}/TurbulenceModels/compressible/lnInclude"
    "${FOAM_SRC}/transportModels"
    "${FOAM_SRC}/transportModels/incompressible/singlePhaseTransportModel"
    "${FOAM_SRC}/transportModels/compressible/lnInclude"
    "${FOAM_SRC}/fileFormats/lnInclude"
    "${FOAM_SRC}/surfMesh/lnInclude"
    "${FOAM_SRC}/lagrangian/basic/lnInclude"
)

set(OPENFOAM_LIBRARY_DIRS "${FOAM_LIBBIN}")
set(OPENFOAM_LIBRARIES
    OpenFOAM
    finiteVolume
    meshTools
    dynamicMesh
    fluidThermophysicalModels
    solidThermo
    specie
    turbulenceModels
    incompressibleTurbulenceModels
    compressibleTurbulenceModels
    incompressibleTransportModels
    compressibleTransportModels
    fileFormats
    surfMesh
    lagrangian
)

# ============================================================================
# Status messages
# ============================================================================
message(STATUS "Found OpenFOAM: ${FOAM_SRC}")
message(STATUS "OpenFOAM libraries: ${FOAM_LIBBIN}")
if(OPENFOAM_VERSION)
    message(STATUS "OpenFOAM version: ${OPENFOAM_VERSION}")
endif()
message(STATUS "WM_LABEL_SIZE: $ENV{WM_LABEL_SIZE}")
message(STATUS "WM_PRECISION_OPTION: $ENV{WM_PRECISION_OPTION}")
message(STATUS "FOAM_API: $ENV{FOAM_API}")

# ============================================================================
# Standard CMake find module
# ============================================================================
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenFOAM
    FOUND_VAR OPENFOAM_FOUND
    REQUIRED_VARS FOAM_SRC FOAM_LIBBIN OPENFOAM_INCLUDE_DIR OPENFOAM_CORE_LIBRARY
    VERSION_VAR OPENFOAM_VERSION
)