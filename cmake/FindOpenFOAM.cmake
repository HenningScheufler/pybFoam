# CMake module to find OpenFOAM
# This module defines:
#  OPENFOAM_FOUND - System has OpenFOAM
#  OPENFOAM_INCLUDE_DIRS - OpenFOAM include directories
#  OPENFOAM_LIBRARIES - OpenFOAM libraries
#  OPENFOAM_LIBRARY_DIRS - OpenFOAM library directories

if(DEFINED ENV{FOAM_SRC})
    set(FOAM_SRC "$ENV{FOAM_SRC}")
    set(FOAM_LIBBIN "$ENV{FOAM_LIBBIN}")
    set(FOAM_USER_LIBBIN "$ENV{FOAM_USER_LIBBIN}")
    
    # Find OpenFOAM include directories
    find_path(OPENFOAM_INCLUDE_DIR
        NAMES fvCFD.H
        PATHS "${FOAM_SRC}/finiteVolume/lnInclude"
        NO_DEFAULT_PATH
    )
    
    if(OPENFOAM_INCLUDE_DIR)
        set(OPENFOAM_INCLUDE_DIRS
            "${FOAM_SRC}/finiteVolume/lnInclude"
            "${FOAM_SRC}/meshTools/lnInclude"
            "${FOAM_SRC}/OpenFOAM/lnInclude"
            "${FOAM_SRC}/OSspecific/POSIX/lnInclude"
            "${FOAM_SRC}/transportModels/lnInclude"
            "${FOAM_SRC}/turbulenceModels/lnInclude"
            "${FOAM_SRC}/thermophysicalModels/lnInclude"
        )
        
        # Find OpenFOAM libraries
        find_library(OPENFOAM_LIBRARY
            NAMES OpenFOAM
            PATHS "${FOAM_LIBBIN}"
            NO_DEFAULT_PATH
        )
        
        if(OPENFOAM_LIBRARY)
            set(OPENFOAM_LIBRARY_DIRS "${FOAM_LIBBIN}")
            set(OPENFOAM_LIBRARIES
                OpenFOAM
                finiteVolume
                meshTools
                transportModels
                turbulenceModels
                thermophysicalModels
            )
            
            # Check OpenFOAM version
            if(EXISTS "${FOAM_SRC}/OpenFOAM/lnInclude/foamVersion.H")
                file(READ "${FOAM_SRC}/OpenFOAM/lnInclude/foamVersion.H" FOAM_VERSION_FILE)
                string(REGEX MATCH "#define OPENFOAM ([0-9]+)" FOAM_VERSION_MATCH "${FOAM_VERSION_FILE}")
                if(FOAM_VERSION_MATCH)
                    set(OPENFOAM_VERSION ${CMAKE_MATCH_1})
                endif()
            endif()
            
            set(OPENFOAM_FOUND TRUE)
            message(STATUS "Found OpenFOAM: ${FOAM_SRC}")
            if(OPENFOAM_VERSION)
                message(STATUS "OpenFOAM version: ${OPENFOAM_VERSION}")
            endif()
        else()
            set(OPENFOAM_FOUND FALSE)
            message(FATAL_ERROR "OpenFOAM libraries not found in ${FOAM_LIBBIN}")
        endif()
    else()
        set(OPENFOAM_FOUND FALSE)
        message(FATAL_ERROR "OpenFOAM headers not found in ${FOAM_SRC}")
    endif()
else()
    set(OPENFOAM_FOUND FALSE)
    message(FATAL_ERROR "OpenFOAM environment not sourced. Please source OpenFOAM before running CMake.")
endif()

# Handle standard CMake arguments
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenFOAM
    FOUND_VAR OPENFOAM_FOUND
    REQUIRED_VARS OPENFOAM_INCLUDE_DIRS OPENFOAM_LIBRARIES
    VERSION_VAR OPENFOAM_VERSION
)
