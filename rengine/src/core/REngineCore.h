#pragma once
#include <platform/RWindows.h>
#include <rendering/RRenderer.h>
#include <core/RTime.h>

using REngine::RRenderer;

using REngine::RWindows;

using REngine::RTime;

namespace REngine {
    class REngineCore {
    public:
        static REngineCore* Init();
    };
}