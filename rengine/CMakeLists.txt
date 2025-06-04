# Real library (not INTERFACE since you have .cpp files)

# 1. REngine library.
add_library(rengine STATIC
        src/core/REngineCore.cpp
        src/core/RTime.cpp
        src/platform/RWindows.cpp
        src/renderers/VulkanRenderer.cpp
)

# 1. REngine includes.
target_include_directories(rengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# 3. Diligent Engine includes.
target_include_directories(rengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL)

# 4. Integrate SDL2.
find_package(SDL2 REQUIRED)

target_include_directories(rengine PRIVATE
        ${IMGUI_DIR}
)

# Find Vulkan SDK
find_package(Vulkan REQUIRED)

# Include directories
target_include_directories(rengine PUBLIC
        ${Vulkan_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
        include/
)


# Add these definitions to enable SDL Vulkan functions
add_definitions(-DSDL_VIDEO_VULKAN)


# Link to your target
#target_link_libraries(your_target PRIVATE imgui)

# 4. Diligent Engine libraries.
target_link_libraries(rengine PUBLIC
        ${Vulkan_LIBRARIES}
        ${SDL2_LIBRARIES}
)
