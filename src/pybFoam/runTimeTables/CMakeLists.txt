# runTimeTables module CMakeLists.txt

set(RUNTIME_TABLES_SOURCES
    foam_runTimeTables.C
    runTimeTables.C
)

set(RUNTIME_TABLES_HEADERS
    foam_runTimeTables.H
)

# Create the pybind11 module
pybind11_add_module(runTimeTables ${RUNTIME_TABLES_SOURCES})

# Set target properties
set_target_properties(runTimeTables PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
)

# Link OpenFOAM libraries
target_link_libraries(runTimeTables PRIVATE
    ${OPENFOAM_LIBRARIES}
)

# Add include directories specific to this module
target_include_directories(runTimeTables PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENFOAM_INCLUDE_DIRS}
)

# Install the module
install(TARGETS runTimeTables
    LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
)
