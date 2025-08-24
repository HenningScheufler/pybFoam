# pybFoam_core module CMakeLists.txt

set(PYBFOAM_CORE_SOURCES
    bind_io.cpp
    bind_dict.cpp
    bind_mesh.cpp
    bind_primitives.cpp
    bind_dimensioned.cpp
    bind_fields.cpp
    bind_geo_fields.cpp
    bind_fvMatrix.cpp
    bind_control.cpp
    bind_cfdTools.cpp
    pybFoam.cpp
)

set(PYBFOAM_CORE_HEADERS
    bind_io.hpp
    bind_dict.hpp
    bind_mesh.hpp
    bind_primitives.hpp
    bind_dimensioned.hpp
    bind_fields.hpp
    bind_geo_fields.hpp
    bind_fvMatrix.hpp
    bind_control.hpp
    bind_cfdTools.hpp
)

# Create the pybind11 module
pybind11_add_module(pybFoam_core ${PYBFOAM_CORE_SOURCES})

# Set target properties
set_target_properties(pybFoam_core PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
)

# Link OpenFOAM libraries
target_link_libraries(pybFoam_core PRIVATE
    OpenFOAM::finiteVolume
)

# Add include directories specific to this module
target_include_directories(pybFoam_core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install the module
if(DEFINED SKBUILD)
    # scikit-build-core manages the installation
    install(TARGETS pybFoam_core
        LIBRARY DESTINATION pybFoam
        COMPONENT python
    )
else()
    # Standalone build - install to Python site-packages
    install(TARGETS pybFoam_core
        LIBRARY DESTINATION "${Python_SITELIB}/pybFoam"
    )
endif()
