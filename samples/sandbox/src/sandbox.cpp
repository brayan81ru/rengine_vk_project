#include <iostream>
#include <core/REngineCore.h>

int main() {
    REngine::REngineCore::Init();

    RWindows window("Sandbox", 1280, 720);

    auto sdlWindow = window.GetNativeWindow();

    const Diligent::NativeWindow nativeWindow = RWindows::SDLWindowToNativeWindow(sdlWindow);

    RRenderer renderer(REngine::RRenderAPI::Direct3D12, nativeWindow);

    while (window.IsRunning()) {
        window.Run();
        RTime::Update();
        renderer.Clear();
        renderer.RenderTestUI(RTime::GetFPS());
        renderer.Frame();
    }

    return 0;
}