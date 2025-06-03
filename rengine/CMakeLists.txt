# Real library (not INTERFACE since you have .cpp files)

# 1. REngine library.
add_library(rengine STATIC
        src/core/REngineCore.cpp
        src/core/RTime.cpp
        src/core/Vector3.h
        src/core/Color.h
        src/core/Matrix4x4.h
        src/core/Texture.h
        src/core/Shader.h
        src/core/Buffer.h
        src/core/Quaternion.h
        src/rendering/ImguiBackend.cpp
        src/rendering/DisplayManager.cpp
        src/rendering/RRenderer.cpp
        src/platform/RWindows.cpp

)

# 1. REngine includes.
target_include_directories(rengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# 2. Diligent Engine includes.
target_include_directories(rengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/DiligentEngine)


# 3. Diligent Engine includes.
target_include_directories(rengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL)

# 4. Integrate SDL2.
find_package(SDL2 REQUIRED)

target_include_directories(rengine PRIVATE
        ${IMGUI_DIR}
)


# Link to your target
#target_link_libraries(your_target PRIVATE imgui)

# 4. Diligent Engine libraries.
target_link_libraries(rengine PUBLIC
        SDL2::SDL2
        SDL2::SDL2main
        Diligent-Common
        Diligent-Imgui
        Diligent-ShaderTools
        Diligent-GraphicsTools
        Diligent-GraphicsEngineD3D12-static
        Diligent-GraphicsEngineD3D11-static
        Diligent-GraphicsEngineOpenGL-static
        Diligent-GraphicsEngineVk-static
)
