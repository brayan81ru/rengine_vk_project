#include "ImguiBackend.h"
#include <DiligentTools/Imgui/interface/ImGuiImplWin32.hpp>

// Platform-specific includes
#if defined(_WIN32)
#include <Windows.h>
#endif

namespace REngine {

ImguiBackend::~ImguiBackend() {
    ShutdownPlatformBackend();
    if (m_Context) {
        ImGui::DestroyContext(m_Context);
    }
}

void ImguiBackend::Initialize(
    Diligent::IRenderDevice* pDevice,
    Diligent::ISwapChain* pSwapChain,
    void* pNativeWindow,
    Diligent::TEXTURE_FORMAT BackBufferFmt,
    Diligent::TEXTURE_FORMAT DepthBufferFmt)
{
    m_pPlatformHandle = pNativeWindow;
    m_Context = ImGui::CreateContext();
    ImGui::SetCurrentContext(m_Context);

    // Initialize Diligent backend
    Diligent::ImGuiDiligentCreateInfo ImguiDiligentCI;
    ImguiDiligentCI.pDevice = pDevice;
    ImguiDiligentCI.BackBufferFmt = BackBufferFmt;
    ImguiDiligentCI.DepthBufferFmt = DepthBufferFmt;
    m_DiligentBackend.Attach(Diligent::ImGuiImplDiligent::Create(ImguiDiligentCI));

    InitializePlatformBackend();
    SetDarkTheme();
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
}

void ImguiBackend::InitializePlatformBackend() {
#if defined(_WIN32)
    if (m_pPlatformHandle) {
        m_pWin32Impl.Attach(Diligent::ImGuiImplWin32::Create(
            Diligent::ImGuiImplWin32::CreateInfo{static_cast<HWND>(m_pPlatformHandle)}));
    }
#endif
}

void ImguiBackend::ShutdownPlatformBackend() {
#if defined(_WIN32)
    if (m_pWin32Impl) {
        m_pWin32Impl->Shutdown();
        m_pWin32Impl.Release();
    }
#endif
}

void ImguiBackend::NewFrame() {
#if defined(_WIN32)
    if (m_pWin32Impl) {
        m_pWin32Impl->NewFrame();
    }
#endif

    if (m_DiligentBackend) {
        m_DiligentBackend->NewFrame();
    }
    ImGui::NewFrame();
}

void ImguiBackend::Render(Diligent::IDeviceContext* pContext) {
    ImGui::Render();
    if (m_DiligentBackend) {
        m_DiligentBackend->Render(pContext);
    }
}

void ImguiBackend::ProcessInputEvent(void* event) {
    // Implement platform-specific event handling if needed
}

void ImguiBackend::SetDarkTheme() {
    ImGui::StyleColorsDark();
    auto& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.Colors[ImGuiCol_WindowBg].w = 0.85f;
}

void ImguiBackend::EnableDocking(bool enable) {
    auto& io = ImGui::GetIO();
    if (enable) {
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    } else {
        io.ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
    }
}

} // namespace REngine