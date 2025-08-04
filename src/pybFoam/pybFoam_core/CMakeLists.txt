# pybFoam_core module CMakeLists.txt

set(PYBFOAM_CORE_SOURCES
    foam_dict.C
    foam_mesh.C
    foam_primitives.C
    foam_dimensioned.C
    foam_fields.C
    foam_geo_fields.C
    foam_fvMatrix.C
    foam_control.C
    foam_cfdTools.C
    pybFoam.C
)

set(PYBFOAM_CORE_HEADERS
    foam_dict.H
    foam_mesh.H
    foam_primitives.H
    foam_dimensioned.H
    foam_fields.H
    foam_geo_fields.H
    foam_fvMatrix.H
    foam_control.H
    foam_cfdTools.H
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
