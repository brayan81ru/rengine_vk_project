#pragma once
#include <NativeWindow.h>
#include <RefCntAutoPtr.hpp>
#include <RenderDevice.h>
#include <rendering/ImguiBackend.h>
#include "vector"

namespace REngine {

    struct RFullScreenMode {
        enum Value {
            ExclusiveFullScreen,
            FullScreenWindow,
            MaximizedWindow,
            Windowed,
            Count
        };

        Value value;
        // Implicit conversion
        constexpr RFullScreenMode(Value v = ExclusiveFullScreen) : value(v) {}
        operator Value() const { return value; }

        // Disallow implicit conversions from other types
        explicit operator bool() = delete;

        // String conversion
        const char* ToString() const {
            static const char* names[] = {
                "Exclusive FullScreen",
                "FullScreen Window",
                "Maximized Window",
                "Windowed"
            };
            return (value < Count) ? names[value] : "Unknown";
        }

        // Optional bonus methods
        static constexpr size_t CountValues() { return Count; }
        static const char* GetName(const size_t index) {
            static const char* names[] = {
                "ExclusiveFullScreen",
                "FullScreenWindow",
                "MaximizedWindow",
                "Windowed"
            };
            return (index < Count) ? names[index] : "Unknown";
        }
    };

    struct RRenderAPI {
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
        constexpr RRenderAPI(Value v = OpenGL) : value(v) {}
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

    struct RVideoMode {
        int width;
        int height;
        int refreshRate;
    };

    struct  RVIdeoConfig {
        std::vector<RVideoMode> AvailableVideoModes;
        RRenderAPI RenderApi;
        RFullScreenMode FullScreenMode;
        int VideoMode;
        int FPSLimit{};
    };




    class RRenderer {
    public:
        RRenderer(RRenderAPI RenderApi, Diligent::NativeWindow NativeWindowHandle);

        void RenderStatsUI(float fps, float frameTime) const;

        void ProcessStatsUIEvents(const SDL_Event *event) const;

        void Clear() const;

        void Frame() const;

    private:
        Diligent::NativeWindow m_Window;
        RRenderAPI m_RenderAPI = RRenderAPI::OpenGL;
        Diligent::SwapChainDesc SCDesc;
        Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pDevice;
        Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
        Diligent::RefCntAutoPtr<Diligent::ISwapChain>     m_pSwapChain;
        ImguiBackend *m_ImguiBackend;
        RVideoMode m_VideoMode;

        void InitializeRendererD3D11();

        void InitializeRendererD3D12();

        void InitializeRendererVulkan();

        void InitializeRendererOpenGL();

    };
}
