#include "RRenderer.h"
#include <iostream>
#include <EngineFactoryD3D11.h>
#include <EngineFactoryD3D12.h>
#include <EngineFactoryOpenGL.h>
#include <EngineFactoryVk.h>
#include <RefCntAutoPtr.hpp>
#include <imgui.h>

namespace REngine {

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


    void RRenderer::InitializeTriangleResources() {

        Diligent::GraphicsPipelineStateCreateInfo PSOCreateInfo;

        // Pipeline state name is used by the engine to report issues.
        // It is always a good idea to give objects descriptive names.
        PSOCreateInfo.PSODesc.Name = "Simple triangle PSO";

        // This is a graphics pipeline
        PSOCreateInfo.PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;

        // clang-format off
        // This tutorial will render to a single render target
        PSOCreateInfo.GraphicsPipeline.NumRenderTargets             = 1;
        // Set render target format which is the format of the swap chain's color buffer
        PSOCreateInfo.GraphicsPipeline.RTVFormats[0]                = m_pSwapChain->GetDesc().ColorBufferFormat;
        // Use the depth buffer format from the swap chain
        PSOCreateInfo.GraphicsPipeline.DSVFormat                    = m_pSwapChain->GetDesc().DepthBufferFormat;
        // Primitive topology defines what kind of primitives will be rendered by this pipeline state
        PSOCreateInfo.GraphicsPipeline.PrimitiveTopology            = Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        // No back face culling for this tutorial
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode      = Diligent::CULL_MODE_NONE;
        // Disable depth testing
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = Diligent::False;
        // clang-format on

        Diligent::ShaderCreateInfo ShaderCI;
        // Tell the system that the shader source code is in HLSL.
        // For OpenGL, the engine will convert this into GLSL under the hood.
        ShaderCI.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
        // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
        ShaderCI.Desc.UseCombinedTextureSamplers = true;
        // Create a vertex shader
        Diligent::RefCntAutoPtr<Diligent::IShader> pVS;
        {
            ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_VERTEX;
            ShaderCI.EntryPoint      = "main";
            ShaderCI.Desc.Name       = "Triangle vertex shader";
            ShaderCI.Source          = VSSource;
            m_pDevice->CreateShader(ShaderCI, &pVS);
        }

        // Create a pixel shader
        Diligent::RefCntAutoPtr<Diligent::IShader> pPS;
        {
            ShaderCI.Desc.ShaderType = Diligent::SHADER_TYPE_PIXEL;
            ShaderCI.EntryPoint      = "main";
            ShaderCI.Desc.Name       = "Triangle pixel shader";
            ShaderCI.Source          = PSSource;
            m_pDevice->CreateShader(ShaderCI, &pPS);
        }

        // Finally, create the pipeline state
        PSOCreateInfo.pVS = pVS;
        PSOCreateInfo.pPS = pPS;
        m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPSO);
    }

    // Render a frame
    void RRenderer::RenderTriangle()
    {
        // Set the pipeline state in the immediate context
        m_pImmediateContext->SetPipelineState(m_pPSO);
        Diligent::DrawAttribs drawAttrs;
        drawAttrs.NumVertices = 3; // We will render 3 vertices
        m_pImmediateContext->Draw(drawAttrs);
    }

    RRenderer::RRenderer(const RenderAPI RenderApi, const Diligent::NativeWindow NativeWindowHandle) {
        std::cout << "RRenderer::Init" << std::endl;

        m_Window = NativeWindowHandle;

        m_RenderAPI = RenderApi;

        switch (RenderApi) {

            case RenderAPI::Direct3D11: InitializeRendererD3D11(); break;

            case RenderAPI::Direct3D12: InitializeRendererD3D12(); break;

            case RenderAPI::OpenGL: InitializeRendererOpenGL(); break;

            case RenderAPI::Vulkan: InitializeRendererVulkan(); break;

            default: {
                std::cout << "Render API not supported" << std::endl;
            };
        }

        // Initialization
        m_ImguiBackend = new ImguiBackend();
        m_ImguiBackend->Initialize(m_pDevice, m_pImmediateContext, m_pSwapChain);
    }

    void RRenderer::RenderStatsUI(const float fps, const float frameTime, DisplayMode displayMode, bool vSync) const {
        m_ImguiBackend->BeginFrame(m_pSwapChain);

        ImGui::Begin("STATS",0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::Text("%s",m_RenderAPI.ToString());
        ImGui::Text("%s",displayMode.ToString());
        ImGui::Text("V-Sync: %s",vSync ? "Enabled" : "Disabled");
        ImGui::Text("FPS: %.2f", fps);
        ImGui::Text("Frametime(ms): %.2f", frameTime);
        ImGui::End();
        m_ImguiBackend->EndFrame(m_pImmediateContext);
    }

    void RRenderer::ProcessStatsUIEvents(const SDL_Event *event) const {
        m_ImguiBackend->ProcessSDLEvent(event);
    }

    void RRenderer::Clear() const {

        constexpr float ClearColor[] = {0.f, 0.f, 0.f, 1.0f};

        // 1. Get views
        Diligent::ITextureView* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
        Diligent::ITextureView* pDSV = m_pSwapChain->GetDepthBufferDSV();

        // 2. Bind targets FIRST (optimal path)
        m_pImmediateContext->SetRenderTargets(
            1, &pRTV, pDSV,
            Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION
        );

        // 3. Now clear (will use fast path)
        m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor,
            Diligent::RESOURCE_STATE_TRANSITION_MODE_NONE); // No need for another transition

        m_pImmediateContext->ClearDepthStencil(pDSV,
            Diligent::CLEAR_DEPTH_FLAG, 1.f, 0,
            Diligent::RESOURCE_STATE_TRANSITION_MODE_NONE);
    }

    void RRenderer::Frame() const {
        m_pSwapChain->Present(m_Vsync ? 1 : 0);
    }

    void RRenderer::SetVSync(const bool vsync) {
        m_Vsync = vsync;
    }

    void RRenderer::InitializeRendererD3D11() {
        const Diligent::EngineD3D11CreateInfo EngineCI;
        auto* pFactoryD3D11 = Diligent::GetEngineFactoryD3D11();
        pFactoryD3D11->CreateDeviceAndContextsD3D11(EngineCI, &m_pDevice, &m_pImmediateContext);
        const Diligent::Win32NativeWindow Window{m_Window};
        pFactoryD3D11->CreateSwapChainD3D11(m_pDevice, m_pImmediateContext, SCDesc, Diligent::FullScreenModeDesc{}, Window, &m_pSwapChain);
        m_pEngineFactory = pFactoryD3D11;
    }

    void RRenderer::InitializeRendererD3D12() {
        const Diligent::EngineD3D12CreateInfo EngineCI;
        auto* pFactoryD3D12 = Diligent::GetEngineFactoryD3D12();
        pFactoryD3D12->CreateDeviceAndContextsD3D12(EngineCI, &m_pDevice, &m_pImmediateContext);
        const Diligent::Win32NativeWindow Window{m_Window};
        pFactoryD3D12->CreateSwapChainD3D12(m_pDevice, m_pImmediateContext, SCDesc, Diligent::FullScreenModeDesc{}, Window, &m_pSwapChain);
        m_pEngineFactory = pFactoryD3D12;
    }

    void RRenderer::InitializeRendererVulkan() {
        const Diligent::EngineVkCreateInfo EngineCI;
        auto* pFactoryVk = Diligent::GetEngineFactoryVk();
        pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &m_pDevice, &m_pImmediateContext);
        const Diligent::Win32NativeWindow Window{m_Window};
        pFactoryVk->CreateSwapChainVk(m_pDevice, m_pImmediateContext, SCDesc, Window, &m_pSwapChain);
        m_pEngineFactory = pFactoryVk;
    }

    void RRenderer::InitializeRendererOpenGL() {
        auto* pFactoryOpenGL = Diligent::GetEngineFactoryOpenGL();
        Diligent::EngineGLCreateInfo EngineCI;
        const Diligent::Win32NativeWindow Window{m_Window};
        EngineCI.Window.hWnd = Window.hWnd;
        pFactoryOpenGL->CreateDeviceAndSwapChainGL(EngineCI, &m_pDevice, &m_pImmediateContext,SCDesc, &m_pSwapChain);
        m_pEngineFactory = pFactoryOpenGL;
    }

}
