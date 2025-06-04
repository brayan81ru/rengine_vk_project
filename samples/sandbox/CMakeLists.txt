# 1 Executable.
add_executable(sandbox src/sandbox.cpp)

# 2 REngine Libraries.
target_link_libraries(sandbox PRIVATE rengine)
