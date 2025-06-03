#include "RRenderer.h"
#include <iostream>
#include <EngineFactoryD3D11.h>
#include <EngineFactoryD3D12.h>
#include <EngineFactoryOpenGL.h>
#include <EngineFactoryVk.h>
#include <RefCntAutoPtr.hpp>
#include <imgui.h>

namespace REngine {
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

        ImGui::Begin("STATS",0);
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
