#pragma once
#include <platform/RWindows.h>
#include <rendering/ImguiBackend.h>
#include <rendering/DisplayManager.h>
#include <rendering/RRenderer.h>
#include <core/RTime.h>
#include <core/Color.h>
#include <core/Vector3.h>
#include <core/Quaternion.h>
#include <core/Matrix4x4.h>
#include <core/Texture.h>
#include <core/Shader.h>
#include <core/Buffer.h>
#include <core/Material.h>
#include <core/ResourceManager.h>

using REngine::RRenderer;

using REngine::RWindows;

using REngine::ImguiBackend;

using REngine::RTime;

using REngine::DisplayManager;

using REngine::Vector3;

using REngine::Quaternion;

using REngine::Color;

using REngine::Matrix4x4;

using REngine::Texture;

using REngine::Shader;

using REngine::Buffer;

using REngine::Material;

using REngine::ResourceManager;

namespace REngine {
    class REngineCore {
    public:
        static REngineCore* Init();
    };
}