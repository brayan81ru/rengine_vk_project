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

    // Sey VSync.
    renderer.SetVSync(true);


    static const char* VSSource = R"(
    struct PSInput
    {
        float4 Pos   : SV_POSITION;
        float3 Color : COLOR;
    };

    void main(in  uint    VertId : SV_VertexID,
              out PSInput PSIn)
    {
        float4 Pos[3];
        Pos[0] = float4(-0.5, -0.5, 0.0, 1.0);
        Pos[1] = float4( 0.0, +0.5, 0.0, 1.0);
        Pos[2] = float4(+0.5, -0.5, 0.0, 1.0);

        float3 Col[3];
        Col[0] = float3(1.0, 0.0, 0.0); // red
        Col[1] = float3(0.0, 1.0, 0.0); // green
        Col[2] = float3(0.0, 0.0, 1.0); // blue

        PSIn.Pos   = Pos[VertId];
        PSIn.Color = Col[VertId];
    }
    )";

    // Pixel shader simply outputs interpolated vertex color
    static const char* PSSource = R"(
    struct PSInput
    {
        float4 Pos   : SV_POSITION;
        float3 Color : COLOR;
    };

    struct PSOutput
    {
        float4 Color : SV_TARGET;
    };

    void main(in  PSInput  PSIn,
              out PSOutput PSOut)
    {
        PSOut.Color = float4(PSIn.Color.rgb, 1.0);
    }
    )";

    // Initialize the texture manager class.
    TextureManager::GetInstance().Initialize(renderer.GetDevice(),renderer.GetDeviceContext());

    Shader shader;

    /*
    if (!shader.LoadFromMemory(VSSource,PSSource)){
        std::cerr << "Failed to load shaders" << std::endl;
        return 1;
    }
    */

    if (!shader.Load("./assets/shaders/shaderTestTriangle.vs", "./assets/shaders/shaderTestTriangle.ps")) {
        std::cerr << "Failed to load shaders" << std::endl;
        return 1;
    }

    while (window.IsRunning()) {
        window.Run();
        RTime::Update();
        renderer.Clear();

        shader.Bind();

        Diligent::DrawAttribs drawAttrs;
        drawAttrs.NumVertices = 3; // We will render 3 vertices
        renderer.GetDeviceContext()->Draw(drawAttrs);

        renderer.RenderStatsUI(RTime::GetFPS(),RTime::GetFrameTimeMS(), displayMode, true);
        renderer.ProcessStatsUIEvents(window.SDL_GetEvent());
        renderer.Frame();
    }

    return 0;
}