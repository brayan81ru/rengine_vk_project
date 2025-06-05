#include <iostream>

#include <core/REngineCore.h>

int main() {
    REngine::REngineCore::Init();

    RWindows window("Sandbox", 1280, 720);

    DisplayManager::Initialize();

    auto currentDisplayMode = DisplayManager::GetCurrentMode();

    DisplayManager::ApplyDisplayMode(window.GetNativeWindow(), nullptr, currentDisplayMode,REngine::FullScreenMode::FullScreenWindow);

    VulkanRenderer renderer;

    if (!renderer.Initialize(window.GetNativeWindow())) {
        std::cerr << "Renderer initialization failed!" << std::endl;
        SDL_DestroyWindow(window.GetNativeWindow());
        SDL_Quit();
        return 1;
    }

    while (window.IsRunning()) {
        window.Run();
        RTime::Update();
        renderer.BeginFrame();
        renderer.EndFrame();
    }

    renderer.Shutdown();
    SDL_DestroyWindow(window.GetNativeWindow());
    SDL_Quit();

    return 0;
}
