#pragma once
#include <memory>
#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>
#include <DiligentTools/Imgui/interface/ImGuiImplDiligent.hpp>
#include <imgui.h>

// Forward declarations
namespace Diligent {
    class ImGuiImplWin32;
}

namespace REngine {
    class ImguiBackend {
    public:
        ~ImguiBackend();

        void Initialize(
            Diligent::IRenderDevice* pDevice,
            Diligent::ISwapChain* pSwapChain,
            void* pNativeWindow,
            Diligent::TEXTURE_FORMAT BackBufferFmt = Diligent::TEX_FORMAT_RGBA8_UNORM,
            Diligent::TEXTURE_FORMAT DepthBufferFmt = Diligent::TEX_FORMAT_UNKNOWN
        );

        void NewFrame();
        void Render(Diligent::IDeviceContext* pContext);
        void ProcessInputEvent(void* event);

        static void SetDarkTheme();
        void EnableDocking(bool enable);

    private:
        void InitializePlatformBackend();
        void ShutdownPlatformBackend();

        Diligent::RefCntAutoPtr<Diligent::ImGuiImplDiligent> m_DiligentBackend;
        ImGuiContext* m_Context = nullptr;
        void* m_pPlatformHandle = nullptr;

#if defined(_WIN32)
        Diligent::RefCntAutoPtr<Diligent::ImGuiImplWin32> m_pWin32Impl;
#endif
    };
} // namespace REngine