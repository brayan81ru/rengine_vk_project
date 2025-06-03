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

    DisplayManager::ApplyDisplayMode(sdlWindow,DisplayManager::GetDesktopMode(0),REngine::FullScreenMode::ExclusiveFullScreen);

    RRenderer renderer(REngine::RenderAPI::Direct3D12, nativeWindow);

    while (window.IsRunning()) {
        window.Run();
        RTime::Update();
        renderer.Clear();
        renderer.RenderStatsUI(RTime::GetFPS(),RTime::GetFrameTimeMS());
        renderer.ProcessStatsUIEvents(window.SDL_GetEvent());
        renderer.Frame();
    }

    return 0;
}