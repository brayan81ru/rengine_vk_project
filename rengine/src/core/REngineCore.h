#pragma once
#include <platform/RWindows.h>
#include <core/RTime.h>
#include <renderers/VulkanRenderer.h>
#include <renderers/DisplayManager.h>

using REngine::RWindows;

using REngine::RTime;

using REngine::VulkanRenderer;

using REngine::DisplayManager;

namespace REngine {
    class REngineCore {
    public:
        static REngineCore* Init();
    };
}