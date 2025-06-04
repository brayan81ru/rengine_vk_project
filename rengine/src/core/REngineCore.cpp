#include "REngineCore.h"
#include "renderers/VulkanRenderer.h"
#include <iostream>

using REngine::VulkanRenderer;

namespace REngine {
    REngineCore * REngineCore::Init() {
        std::cout << "Initializing REngineCore" << std::endl;
        RTime::Init();
        return new REngineCore();
    }
}
