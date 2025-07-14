# Embedding Python library CMakeLists.txt

set(EMBED_PYTHON_SOURCES
    pyInterp.C
)

set(EMBED_PYTHON_HEADERS
    pyInterp.H
)

# Create shared library
add_library(embPython SHARED ${EMBED_PYTHON_SOURCES})

# Set target properties
set_target_properties(embPython PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN ON
)

# Link OpenFOAM and Python libraries
target_link_libraries(embPython PUBLIC
    OpenFOAM::api
    Python::Python
)

# Add include directories
target_include_directories(embPython PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install the library
install(TARGETS embPython
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Install headers
install(FILES ${EMBED_PYTHON_HEADERS}
    DESTINATION include/embeddingPython
)
