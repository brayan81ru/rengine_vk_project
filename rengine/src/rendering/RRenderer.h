#pragma once
#include <NativeWindow.h>
#include <RefCntAutoPtr.hpp>
#include <RenderDevice.h>
#include <rendering/ImguiBackend.h>
#include <rendering/DisplayManager.h>

namespace REngine {

    struct RenderAPI {
        // The actual enum values
        enum Value {
            OpenGL,
            Direct3D11,
            Direct3D12,
            Vulkan,
            Count  // Useful for iteration/validation
        };

        Value value;

        // Implicit conversion
        constexpr RenderAPI(Value v = OpenGL) : value(v) {}
        operator Value() const { return value; }

        // Disallow implicit conversions from other types
        explicit operator bool() = delete;

        // String conversion
        const char* ToString() const {
            static const char* names[] = {
                "OpenGL",
                "Direct3D11",
                "Direct3D12",
                "Vulkan"
            };
            return (value < Count) ? names[value] : "Unknown";
        }

        // Optional bonus methods
        static constexpr size_t CountValues() { return Count; }
        static const char* GetName(const size_t index) {
            static const char* names[] = {
                "OpenGL",
                "Direct3D11",
                "Direct3D12",
                "Vulkan"
            };
            return (index < Count) ? names[index] : "Unknown";
        }
    };

    class RRenderer {
    public:
        void InitializeTriangleResources();

        void RenderTriangle();

        RRenderer(RenderAPI RenderApi, Diligent::NativeWindow NativeWindowHandle);

        void RenderStatsUI(float fps, float frameTime, DisplayMode displayMode, bool vSync) const;

        void ProcessStatsUIEvents(const SDL_Event *event) const;

        void Clear() const;

        void Frame() const;

        void SetVSync(bool vsync);

    private:
        bool m_Vsync = true;
        Diligent::NativeWindow m_Window;
        RenderAPI m_RenderAPI = RenderAPI::OpenGL;
        Diligent::SwapChainDesc SCDesc;
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>     m_pSwapChain;
        ImguiBackend *m_ImguiBackend;
        Diligent::IEngineFactory *m_pEngineFactory;
        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSO;

        void InitializeRendererD3D11();

        void InitializeRendererD3D12();

        void InitializeRendererVulkan();

        void InitializeRendererOpenGL();

        void RecreateSwapChain();
    };
}
