cmake_minimum_required(VERSION 3.31)

project(rengine_vk__project)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        SOURCE_DIR rengine/third_party/glm

)
FetchContent_MakeAvailable(glm)

# Add all subprojects
add_subdirectory(rengine)
add_subdirectory(samples/sandbox)

