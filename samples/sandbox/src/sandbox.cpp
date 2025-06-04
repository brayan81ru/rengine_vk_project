#include <core/REngineCore.h>

int main() {
    REngine::REngineCore::Init();

    RWindows window("Sandbox", 1280, 720);

    while (window.IsRunning()) {
        window.Run();
        RTime::Update();
    }

    return 0;
}