#pragma once
#include <platform/RWindows.h>
#include <core/RTime.h>

using REngine::RWindows;

using REngine::RTime;

namespace REngine {
    class REngineCore {
    public:
        static REngineCore* Init();
    };
}