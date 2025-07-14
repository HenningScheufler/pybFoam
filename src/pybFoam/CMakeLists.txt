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
install(FILES ${PYBFOAM_PYTHON_FILES}
    DESTINATION "${Python_SITELIB}/pybFoam"
)
