// engine/src/renderer/ImguiBackend.h
#pragma once

#include <RefCntAutoPtr.hpp>
#include <ImGuiImplDiligent.hpp>
#include <RenderDevice.h>
#include <DeviceContext.h>
#include <SwapChain.h>

namespace REngine {

    class ImguiBackend {
    public:
        ImguiBackend();
        ~ImguiBackend();

        bool Initialize(
            Diligent::IRenderDevice* device,
            Diligent::IDeviceContext* context,
            Diligent::ISwapChain* swapChain,
            const char* fontPath = nullptr
        );

        void NewFrame(Diligent::ISwapChain* swapChain);

        void BeginFrame(const Diligent::ISwapChain *swapChain) const;

        void EndFrame(Diligent::IDeviceContext *context) const;

        void Render(Diligent::IDeviceContext* context);
        void Shutdown();

        // Handle platform events (Windows, Linux, Mac)
        bool HandleEvent(const void* eventData) const;

    private:
        std::unique_ptr<Diligent::ImGuiImplDiligent> m_ImGuiImpl;
        bool m_Initialized = false;
    };

} // namespace REngine