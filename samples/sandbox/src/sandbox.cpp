#include <core/REngineCore.h>

int main() {
    REngine::REngineCore::Init();

    // Create transformation matrix
    Vector3 position(1, 2, 3);
    Quaternion rotation = Quaternion<>::FromEuler(0, 45, 0);
    Vector3 scale(1, 1, 1);

    Matrix4x4 worldMatrix = Matrix4x4::TRS(position, rotation, scale);

    // Initialize Window&Rendering
    RWindows window("Sandbox", 1280, 720);

    auto sdlWindow = window.GetNativeWindow();

    const Diligent::NativeWindow nativeWindow = RWindows::SDLWindowToNativeWindow(sdlWindow);

    // Display Manager Initializer.
    DisplayManager::Initialize();

    auto modes = DisplayManager::GetAvailableModes(0);

    auto displayMode = DisplayManager::GetCurrentMode(0);

    // Set the display mode.
    DisplayManager::ApplyDisplayMode(&window,displayMode,REngine::FullScreenMode::FullScreenWindow);

    // Initialize the renderer.
    RRenderer renderer(REngine::RenderAPI::Direct3D12, nativeWindow);

    // Set VSync.
    renderer.SetVSync(true);

    // Initialize the texture manager class.
    TextureManager::GetInstance().Initialize(renderer.GetDevice(),renderer.GetDeviceContext());

    // Initialize and load the vertex shaders and pixel shaders.
    Shader shader;


    if (!shader.Load("./assets/shaders/shaderTestTriangle.vs", "./assets/shaders/shaderTestTriangle.ps")) {
        std::cerr << "Failed to load shaders" << std::endl;
        return 1;
    }

    /*
    Shader pbrShader;
    if (!pbrShader.Load("./assets/engine/shaders/PBR.vs", "./assets/engine/shaders/PBR.ps")) {
        std::cerr << "Failed to load PBR shaders" << std::endl;
        return 1;
    }
    */



    while (window.IsRunning()) {
        window.Run();
        RTime::Update();
        renderer.Clear();

        // Bind the shader.
        shader.Bind();

        // Render the triangle.
        Diligent::DrawAttribs drawAttrs;
        drawAttrs.NumVertices = 3; // We will render 3 vertices
        renderer.GetDeviceContext()->Draw(drawAttrs);


        // Render the Stats UI.
        renderer.RenderStatsUI(RTime::GetFPS(),RTime::GetFrameTimeMS(), displayMode, true);
        renderer.ProcessStatsUIEvents(window.SDL_GetEvent());
        renderer.Frame();
    }

    return 0;
}