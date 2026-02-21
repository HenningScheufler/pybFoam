# Dependencies Configuration for pybFoam
# Uses nanobind discovered from the installed Python environment

# Function to find nanobind via the Python executable
function(find_nanobind_from_python)
    if(NOT Python_EXECUTABLE)
        message(FATAL_ERROR "Python_EXECUTABLE not set. Find Python before calling configure_dependencies().")
    endif()

    execute_process(
        COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE NB_CMAKE_DIR
        RESULT_VARIABLE NB_RESULT
    )

    if(NOT NB_RESULT EQUAL 0 OR NOT NB_CMAKE_DIR)
        message(FATAL_ERROR
            "nanobind not found. Install it with: pip install nanobind\n"
            "Got result: ${NB_RESULT}, dir: '${NB_CMAKE_DIR}'"
        )
    endif()

    message(STATUS "nanobind cmake dir: ${NB_CMAKE_DIR}")
    list(APPEND CMAKE_PREFIX_PATH "${NB_CMAKE_DIR}")
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}" PARENT_SCOPE)
    find_package(nanobind CONFIG REQUIRED)
endfunction()

# Main function to configure all dependencies
function(configure_dependencies)
    message(STATUS "Configuring dependencies...")
    find_nanobind_from_python()
    message(STATUS "Dependencies configuration complete")
endfunction()

# Call the main configuration function
configure_dependencies()
