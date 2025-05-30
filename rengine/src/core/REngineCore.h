#pragma once
#include <platform/RWindows.h>
#include <rendering/RRenderer.h>
#include <core/RString.h>

using REngine::RRenderer;

using REngine::RWindows;

using REngine::RString;


namespace REngine {
    class REngineCore {
    public:
        static REngineCore* Init();
    };
}