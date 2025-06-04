cmake_minimum_required(VERSION 3.31)

project(rengine_vk__project)

set(CMAKE_CXX_STANDARD 17)

# Set third_party as base for external projects
set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/rengine/third_party)

# Add all subprojects
add_subdirectory(rengine)
add_subdirectory(samples/sandbox)

