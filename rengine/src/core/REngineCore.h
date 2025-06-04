#pragma once
#include <platform/RWindows.h>
#include <core/RTime.h>
#include <renderers/VulkanRenderer.h>

using REngine::RWindows;

using REngine::RTime;

using REngine::VulkanRenderer;

namespace REngine {
    class REngineCore {
    public:
        static REngineCore* Init();
    };
}