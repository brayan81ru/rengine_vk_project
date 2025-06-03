# 1 Executable.
add_executable(sandbox src/sandbox.cpp)

# 2 REngine Libraries.
target_link_libraries(sandbox PRIVATE rengine)

# Copy shaders to build directory
file(COPY ../../assets/engine/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets/engine)
