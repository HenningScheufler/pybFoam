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
    OpenFOAM::api
)

# Add include directories specific to this module
target_include_directories(fvc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install the module
if(DEFINED SKBUILD)
    # scikit-build-core manages the installation
    install(TARGETS fvc
        LIBRARY DESTINATION pybFoam
        COMPONENT python
    )
else()
    # Standalone build - install to Python site-packages
    install(TARGETS fvc
        LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
    )
endif()
