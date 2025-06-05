#pragma once
#include <platform/RWindows.h>
#include <core/RTime.h>
#include <renderers/VulkanRenderer.h>
#include <renderers/DisplayManager.h>
#include <renderers/Texture.h>

using REngine::RWindows;

using REngine::RTime;

using REngine::VulkanRenderer;

using REngine::DisplayManager;

using REngine::Texture;

namespace REngine {
    class REngineCore {
    public:
        static REngineCore* Init();
    };
}