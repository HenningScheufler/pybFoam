# Main pybFoam module CMakeLists.txt

# Add subdirectories for each module
add_subdirectory(pybFoam_core)
add_subdirectory(runTimeTables)
add_subdirectory(fvc)
add_subdirectory(fvm)
add_subdirectory(thermo)
add_subdirectory(turbulence)

# Create a Python module that includes all components
set(PYBFOAM_PYTHON_FILES
    __init__.py
    _version.py
)

# Install Python files to the package directory
if(DEFINED SKBUILD)
    # scikit-build-core manages the installation
    # Just install to the standard Python package location
    install(FILES ${PYBFOAM_PYTHON_FILES}
        DESTINATION pybFoam
        COMPONENT python
    )
    
else()
    # Standalone build - install to Python site-packages
    install(FILES ${PYBFOAM_PYTHON_FILES}
        DESTINATION "${Python_SITELIB}/pybFoam"
    )
endif()
