#include <iostream>
#include <core/REngineCore.h>

#include "renderers/VulkanRenderer.h"

int main() {
    REngine::REngineCore::Init();

    RWindows window("Sandbox", 1280, 720);

    REngine::VulkanRenderer renderer;

    if (!renderer.Initialize(window.GetNativeWindow())) {
        std::cerr << "Renderer initialization failed!" << std::endl;
        SDL_DestroyWindow(window.GetNativeWindow());
        SDL_Quit();
        return 1;
    }

    while (window.IsRunning()) {
        window.Run();
        RTime::Update();
    }

    return 0;
}
