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

    renderer.InitImGui(window.GetNativeWindow());

    const auto texture = new Texture();
    texture->CreateFromFile(
        renderer.GetDevice(),
        renderer.GetPhysicalDevice(),
        renderer.GetCommandPool(),
        renderer.GetQueue(),
        "d:/test/001.png");

    while (window.IsRunning()) {
        window.Run();
        RTime::Update();
        renderer.ProcessImGuiEvents(window.SDL_GetEvent());
        renderer.BeginFrame();
        renderer.RenderImGui();
        renderer.EndFrame();

        // Handle Device Lost.
        if (renderer.IsDeviceLost()) {
            renderer.HandleDeviceLost();
            continue;
        }
    }

    renderer.ShutdownImGui();
    renderer.Shutdown();
    SDL_DestroyWindow(window.GetNativeWindow());
    SDL_Quit();

    return 0;
}
