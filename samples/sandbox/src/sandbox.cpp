#include <core/REngineCore.h>

int main() {
    REngine::REngineCore::Init();

    // Initialize Window&Rendering
    RWindows window("Sandbox", 1280, 720);

    auto sdlWindow = window.GetNativeWindow();

    const Diligent::NativeWindow nativeWindow = RWindows::SDLWindowToNativeWindow(sdlWindow);

    // Display Manager Initializer.
    DisplayManager::Initialize();

    auto modes = DisplayManager::GetAvailableModes(0);

    auto displayMode = DisplayManager::GetCurrentMode(0);

    DisplayManager::ApplyDisplayMode(&window,displayMode,REngine::FullScreenMode::FullScreenWindow);

    RRenderer renderer(REngine::RenderAPI::Direct3D12, nativeWindow);

    renderer.SetVSync(true);

    renderer.InitializeTriangleResources();

    while (window.IsRunning()) {
        window.Run();
        RTime::Update();
        renderer.Clear();
        renderer.RenderTriangle();
        renderer.RenderStatsUI(RTime::GetFPS(),RTime::GetFrameTimeMS(), displayMode, true);
        renderer.ProcessStatsUIEvents(window.SDL_GetEvent());
        renderer.Frame();
    }

    return 0;
}