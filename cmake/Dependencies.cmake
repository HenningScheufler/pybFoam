# CPM Dependencies Configuration for pybFoam

# CPM.cmake - CMake Package Manager
# This file configures all external dependencies using CPM

# Download and include CPM.cmake
set(CPM_DOWNLOAD_VERSION 0.38.3)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
        ${CPM_DOWNLOAD_LOCATION}
        EXPECTED_HASH SHA256=cc155ce02e7945e7b8967ddfaff0b050e958a723ef7aad3766d368940cb15494
    )
endif()

include(${CPM_DOWNLOAD_LOCATION})

# Configure CPM
set(CPM_USE_LOCAL_PACKAGES ON)
set(CPM_LOCAL_PACKAGES_ONLY OFF)

# Define dependency versions
set(PYBIND11_VERSION 2.11.1)

# Function to add pybind11
function(add_pybind11)
    CPMAddPackage(
        NAME pybind11
        GITHUB_REPOSITORY pybind/pybind11
        VERSION ${PYBIND11_VERSION}
        OPTIONS
            "PYBIND11_INSTALL ON"
            "PYBIND11_TEST OFF"
            "PYBIND11_NOPYTHON OFF"
    )
    
    if(pybind11_ADDED)
        message(STATUS "Added pybind11 ${PYBIND11_VERSION}")
    endif()
endfunction()

# Function to add testing dependencies
function(add_testing_deps)
    # Add Catch2 for C++ testing
    CPMAddPackage(
        NAME Catch2
        GITHUB_REPOSITORY catchorg/Catch2
        VERSION 3.4.0
        OPTIONS
            "CATCH_INSTALL_DOCS OFF"
            "CATCH_INSTALL_EXTRAS OFF"
    )
    
    if(Catch2_ADDED)
        message(STATUS "Added Catch2 for testing")
    endif()
endfunction()

# Main function to configure all dependencies
function(configure_dependencies)
    message(STATUS "Configuring dependencies with CPM...")
    
    # Essential dependencies
    add_pybind11()

    option(PYBFOAM_BUILD_TESTS "Build tests" ON)
    if(PYBFOAM_BUILD_TESTS)
        add_testing_deps()
    endif()
    
    message(STATUS "Dependencies configuration complete")
endfunction()

# Call the main configuration function
configure_dependencies()
