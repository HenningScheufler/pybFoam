# test-pythonNumpy application CMakeLists.txt

# Add executable
add_executable(test-pythonNumpy test-pythonNumpy.C)

# Link libraries
target_link_libraries(test-pythonNumpy
    ${OPENFOAM_LIBRARIES}
    embPython
    Python::Python
)

# Add include directories
target_include_directories(test-pythonNumpy PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENFOAM_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src/embeddingPython/lnInclude
)

# Install the executable
install(TARGETS test-pythonNumpy
    RUNTIME DESTINATION bin
)
