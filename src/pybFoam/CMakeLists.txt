# Main pybFoam module CMakeLists.txt

# --- Canonical nanobind shared library -------------------------------------
# Build one libnanobind.so, shared by every NB_SHARED module in this package.
# Treated as a private implementation detail of the binding modules:
# installed alongside them in pybFoam/, reached via INSTALL_RPATH=$ORIGIN.
# Not exported via install(EXPORT) — downstream nanobind-extension consumers
# (e.g. pyOFTools) get nanobind from their own pip-install of nanobind, and
# rely on Linux loader SONAME deduplication to share the same in-process
# libnanobind.so as pybFoam at runtime (matching ABI flags required).
nanobind_build_library(nanobind)

if(DEFINED SKBUILD)
    install(TARGETS nanobind
        LIBRARY DESTINATION pybFoam
        COMPONENT python)
else()
    install(TARGETS nanobind
        LIBRARY DESTINATION "${Python_SITELIB}/pybFoam")
endif()

# RPATH consumed by every per-module CMakeLists below
set(PYBFOAM_MODULE_INSTALL_RPATH "$ORIGIN")

# Add subdirectories for each module
add_subdirectory(pybFoam_core)
add_subdirectory(runTimeTables)
add_subdirectory(fvc)
add_subdirectory(fvm)
add_subdirectory(thermo)
add_subdirectory(turbulence)
add_subdirectory(sampling)
add_subdirectory(meshing)

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
