#pragma once
#include <NativeWindow.h>
#include <RefCntAutoPtr.hpp>
#include <RenderDevice.h>


namespace REngine {
    enum class RRenderAPI {
        OpenGL,
        Direct3D11,
        Direct3D12,
        Vulkan
    };

    class RRenderer {
    public:
        RRenderer(RRenderAPI RenderApi, Diligent::NativeWindow NativeWindowHandle);


        void Clear() const;
        void Frame() const;
    private:
        Diligent::NativeWindow m_Window;
        RRenderAPI m_RenderAPI = RRenderAPI::OpenGL;
        Diligent::SwapChainDesc SCDesc;
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>     m_pSwapChain;

        void InitializeRendererD3D11();

        void InitializeRendererD3D12();

        void InitializeRendererVulkan();

        void InitializeRendererOpenGL();


    };
}
