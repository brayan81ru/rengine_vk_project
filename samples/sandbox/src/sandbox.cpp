#include <iostream>
#include <core/REngineCore.h>

int main() {

    RString pepe = "Hola pepe!!!";

    RString pepe2 = "Hola pepe2!!!";

    pepe = pepe2;

    pepe = RString::Format("Pepep is {} jajaja.",pepe.c_str());

    std::cout << pepe.c_str()<< std::endl;

    return 0;


    REngine::REngineCore::Init();

    // 1. Create window.
    RWindows window("Sandbox", 1280, 720);

    auto sdlWindow = window.GetNativeWindow();

    const Diligent::NativeWindow nativeWindow = RWindows::SDLWindowToNativeWindow(sdlWindow);

    RRenderer renderer(REngine::RRenderAPI::OpenGL, nativeWindow);

    // 3. Main loop.
    bool shouldQuit = false;

    while (!shouldQuit) {
        window.PollEvents(shouldQuit);
        renderer.Clear();
        renderer.Frame();
    }

    return 0;
}