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
        std::cout << RTime::GetFPS() << std::endl;
        renderer.Clear();
        renderer.Frame();
    }

    return 0;
}