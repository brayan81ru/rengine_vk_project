# Real library (not INTERFACE since you have .cpp files)

# 1. REngine library.
add_library(rengine STATIC
        src/core/REngineCore.cpp
        src/core/RString.cpp
        src/rendering/RRenderer.cpp
        src/platform/RWindows.cpp
)

# 1. REngine includes.
target_include_directories(rengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# 2. Diligent Engine includes.
target_include_directories(rengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/DiligentEngine)

# 2. Diligent Engine includes.
target_include_directories(rengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL)

# 3. Integrate SDL2.
find_package(SDL2 REQUIRED)

# Ensure headers see the correct fmt version
target_include_directories(rengine PRIVATE
        ${FMT_DIR}/include
)

# 4. Diligent Engine libraries.
target_link_libraries(rengine PUBLIC
        fmt::fmt
        SDL2::SDL2
        SDL2::SDL2main
        Diligent-Common
        Diligent-GraphicsEngineD3D12-static
        Diligent-GraphicsEngineD3D11-static
        Diligent-GraphicsEngineOpenGL-static
        Diligent-GraphicsEngineVk-static
)
