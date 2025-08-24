# fvm module CMakeLists.txt

set(FVM_SOURCES
    bind_fvm.cpp
    fvm.cpp
)

set(FVM_HEADERS
    bind_fvm.hpp
)

# Create the pybind11 module
pybind11_add_module(fvm ${FVM_SOURCES})

# Set target properties
set_target_properties(fvm PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
)

# Link OpenFOAM libraries
target_link_libraries(fvm PRIVATE
    OpenFOAM::finiteVolume
)

# Add include directories specific to this module
target_include_directories(fvm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install the module
if(DEFINED SKBUILD)
    # scikit-build-core manages the installation
    install(TARGETS fvm
        LIBRARY DESTINATION pybFoam
        COMPONENT python
    )
else()
    # Standalone build - install to Python site-packages
    install(TARGETS fvm
        LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
    )
endif()
