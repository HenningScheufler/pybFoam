# fvc module CMakeLists.txt

set(FVC_SOURCES
    bind_fvc.cpp
    fvc.cpp
)

set(FVC_HEADERS
    bind_fvc.hpp
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
    OpenFOAM::finiteVolume
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
