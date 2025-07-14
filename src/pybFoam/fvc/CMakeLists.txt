# fvc module CMakeLists.txt

set(FVC_SOURCES
    foam_fvc.C
    fvc.C
)

set(FVC_HEADERS
    foam_fvc.H
)

# Create the pybind11 module
pybind11_add_module(fvc ${FVC_SOURCES})

# Set target properties
set_target_properties(fvc PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
)

# Link OpenFOAM libraries
target_link_libraries(fvc PRIVATE
    ${OPENFOAM_LIBRARIES}
)

# Add include directories specific to this module
target_include_directories(fvc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENFOAM_INCLUDE_DIRS}
)

# Install the module
install(TARGETS fvc
    LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
)
