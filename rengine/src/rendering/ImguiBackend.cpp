// engine/src/renderer/ImguiBackend.cpp
#include "ImguiBackend.h"
#include "imgui.h"

namespace REngine {
    ImguiBackend::ImguiBackend() = default;

    ImguiBackend::~ImguiBackend() {
        Shutdown();
    }

    bool ImguiBackend::Initialize(
        Diligent::IRenderDevice* device,
        Diligent::IDeviceContext* context,
        Diligent::ISwapChain* swapChain,
        const char* fontPath
    ) {
        if (m_Initialized) {
            return true;
        }

        // Initialize ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

        // Setup style
        ImGui::StyleColorsDark();

        // Initialize Diligent ImGui implementation
        const auto& SCDesc = swapChain->GetDesc();
        Diligent::ImGuiDiligentCreateInfo CI;
        CI.pDevice = device;
        CI.BackBufferFmt = SCDesc.ColorBufferFormat;
        CI.DepthBufferFmt = SCDesc.DepthBufferFormat;
        CI.InitialVertexBufferSize = 1024;  // Adjust as needed
        CI.InitialIndexBufferSize = 2048;   // Adjust as needed

        m_ImGuiImpl = std::make_unique<Diligent::ImGuiImplDiligent>(CI);

        // Load custom font if specified
        if (fontPath) {
            ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath, 16.0f);
            IM_ASSERT(font != nullptr);
            // Recreate device objects if font was loaded
            m_ImGuiImpl->CreateDeviceObjects();
        }

        m_Initialized = true;
        return true;
    }

    void ImguiBackend::NewFrame(Diligent::ISwapChain* swapChain) {
        if (!m_Initialized) return;

        const auto& SCDesc = swapChain->GetDesc();

        ImGuiIO& io = ImGui::GetIO();

        io.DisplaySize = ImVec2(
            static_cast<float>(SCDesc.Width),
            static_cast<float>(SCDesc.Height)
        );


        // 2. Begin Diligent frame
        m_ImGuiImpl->NewFrame(
            SCDesc.Width,
            SCDesc.Height,
            SCDesc.PreTransform
        );

        ImGui::NewFrame();
    }

    void ImguiBackend::BeginFrame(const Diligent::ISwapChain* swapChain) const {
        if (!m_Initialized) return;

        // Ensure previous frame was properly ended
        if (ImGui::GetCurrentContext() && ImGui::GetFrameCount() > 0) {
            if (!ImGui::GetIO().WantCaptureMouse && !ImGui::GetIO().WantCaptureKeyboard) {
                //ImGui::EndFrame();
            }
        }

        // Update display size
        const auto& SCDesc = swapChain->GetDesc();
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(
            static_cast<float>(SCDesc.Width),
            static_cast<float>(SCDesc.Height)
        );

        // Begin new frame
        m_ImGuiImpl->NewFrame(
            SCDesc.Width,
            SCDesc.Height,
            SCDesc.PreTransform
        );
    }

    void ImguiBackend::EndFrame(Diligent::IDeviceContext* context) const {
        if (!m_Initialized) return;

        // Only render if we have a valid frame
        if (ImGui::GetCurrentContext() && ImGui::GetFrameCount() > 0) {
            ImGui::Render();
            if (ImGui::GetDrawData()) {
                m_ImGuiImpl->Render(context);
            }
        }
    }


    bool ImguiBackend::HandleEvent(const void* eventData) const {
        if (!m_Initialized) return false;

        return false;
    }

    void ImguiBackend::Shutdown() {
        if (!m_Initialized) return;
        m_ImGuiImpl->InvalidateDeviceObjects();
        m_ImGuiImpl.reset();
        ImGui::DestroyContext();
        m_Initialized = false;
    }
} // namespace REngine