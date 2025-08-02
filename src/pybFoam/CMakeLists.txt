# Main pybFoam module CMakeLists.txt

# Add subdirectories for each module
add_subdirectory(pybFoam_core)
add_subdirectory(runTimeTables)
add_subdirectory(fvc)
add_subdirectory(thermo)
add_subdirectory(turbulence)

# Create a Python module that includes all components
set(PYBFOAM_PYTHON_FILES
    __init__.py
    _version.py
    dict.py
    fieldFunctions.py
    geoFields.py
    postProcess.py
    time_series.py
)

# Install Python files to the package directory
if(DEFINED SKBUILD)
    # scikit-build-core manages the installation
    # Just install to the standard Python package location
    install(FILES ${PYBFOAM_PYTHON_FILES}
        DESTINATION pybFoam
        COMPONENT python
    )
    
    # Add a custom target to generate stubs for pybind11 modules
    find_program(PYBIND11_STUBGEN pybind11-stubgen)
    if(PYBIND11_STUBGEN)
        add_custom_target(generate_stubs
            COMMAND ${PYBIND11_STUBGEN} pybFoam.pybFoam_core -o ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND ${PYBIND11_STUBGEN} pybFoam.fvc -o ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND ${PYBIND11_STUBGEN} pybFoam.thermo -o ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND ${PYBIND11_STUBGEN} pybFoam.turbulence -o ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND ${PYBIND11_STUBGEN} pybFoam.runTimeTables -o ${CMAKE_CURRENT_SOURCE_DIR}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating Python stub files for pybind11 modules"
            DEPENDS pybFoam_core fvc thermo turbulence runTimeTables
        )
        message(STATUS "Added target 'generate_stubs' for Python stub generation")
    else()
        message(WARNING "pybind11-stubgen not found. Install with: pip install pybind11-stubgen")
    endif()
else()
    # Standalone build - install to Python site-packages
    install(FILES ${PYBFOAM_PYTHON_FILES}
        DESTINATION "${Python_SITELIB}/pybFoam"
    )
endif()
