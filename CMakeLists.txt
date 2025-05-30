cmake_minimum_required(VERSION 3.31)
project(rengine_project)

set(CMAKE_CXX_STANDARD 17)

# Set third_party as base for external projects
set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/rengine/third_party)
set(FMT_DIR ${THIRD_PARTY_DIR}/fmt)  # Explicit path

# 1. Option 1: Clone automatically (recommended)
option(FETCH_FMT "Automatically download fmt library" OFF)

if(FETCH_FMT)
    include(FetchContent)
    FetchContent_Declare(
            fmt
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG 9.1.0
            SOURCE_DIR ${FMT_DIR}
    )
    FetchContent_MakeAvailable(fmt)
else()
    # 2. Option 2: Use existing checkout
    add_subdirectory(${FMT_DIR})
endif()



# Add all subprojects
add_subdirectory(rengine/third_party/DiligentEngine)
add_subdirectory(rengine)
add_subdirectory(samples/sandbox)

