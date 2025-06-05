# Real library (not INTERFACE since you have .cpp files)

# 1. REngine library.
add_library(rengine STATIC
        src/core/REngineCore.cpp
        src/core/RTime.cpp
        src/renderers/DisplayManager.cpp
        src/platform/RWindows.cpp
        src/renderers/VulkanRenderer.cpp
        src/renderers/Texture.cpp
        src/renderers/Shader.cpp
        src/core/VulkanBuffer.cpp
)



# 1. REngine includes.
target_include_directories(rengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(rengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/core)
target_include_directories(rengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb)
target_include_directories(rengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/SPIRV)

# 3. Diligent Engine includes.
target_include_directories(rengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL)

# 4. Integrate SDL2.
find_package(SDL2 REQUIRED)

# Find Vulkan SDK
find_package(Vulkan REQUIRED)

# Add these definitions to enable SDL Vulkan functions
add_definitions(-DSDL_VIDEO_VULKAN)

# Include directories
target_include_directories(rengine PUBLIC
        ${Vulkan_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
        include/
)


#ImgUI Requirements.
add_library(imgui STATIC
        third_party/imgui/imgui.cpp
        third_party/imgui/imgui_draw.cpp
        third_party/imgui/imgui_tables.cpp
        third_party/imgui/imgui_widgets.cpp
        third_party/imgui/backends/imgui_impl_vulkan.cpp
        third_party/imgui/backends/imgui_impl_sdl2.cpp
)

#ImgUI required includes.
target_include_directories(imgui PUBLIC
        ${Vulkan_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
)

#ImgUI sources.
target_include_directories(imgui PUBLIC third_party/imgui)
target_include_directories(imgui PUBLIC third_party/glm)


# 4. Diligent Engine libraries.
target_link_libraries(rengine PUBLIC
        ${Vulkan_LIBRARIES}
        ${SDL2_LIBRARIES}
        imgui
)