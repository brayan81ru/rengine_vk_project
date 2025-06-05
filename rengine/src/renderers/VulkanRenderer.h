#pragma once

#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

namespace REngine {

    class VulkanRenderer {
    public:
        VulkanRenderer();

        ~VulkanRenderer();

        bool Initialize(SDL_Window* window);

        void Shutdown();

        bool BeginFrame();

        void EndFrame();

        void CheckVkResult(VkResult result);

        void SetVsync(bool enabled);

        void RecreateSwapchain();

        void HandleDeviceLost();

        [[nodiscard]] bool IsDeviceLost() const { return m_deviceLost; }

        void InitImGui(SDL_Window *window);

        void RenderImGui() const;

        void ProcessImGuiEvents(const SDL_Event *event) const;

        void ShutdownImGui();

    private:

        // Vsync
        bool m_Vsync = true;
        VkPresentModeKHR m_currentPresentMode;

        VkDescriptorPool m_imguiDescriptorPool;
        VkDescriptorPoolCreateInfo m_poolInfo;

        // Constants
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        // Core Vulkan objects
        VkInstance m_instance;
        VkPhysicalDevice m_physicalDevice;
        VkDevice m_device;
        VkSurfaceKHR m_surface;

        // Queues
        VkQueue m_graphicsQueue;
        VkQueue m_presentQueue;
        uint32_t m_graphicsQueueFamilyIndex;
        uint32_t m_presentQueueFamilyIndex;

        // Swapchain
        VkSwapchainKHR m_swapchain;
        std::vector<VkImage> m_swapchainImages;
        std::vector<VkImageView> m_swapchainImageViews;
        VkFormat m_swapchainImageFormat;
        VkExtent2D m_swapchainExtent;

        // Rendering
        VkRenderPass m_renderPass;
        std::vector<VkFramebuffer> m_framebuffers;

        // Command buffers
        VkCommandPool m_commandPool;
        std::vector<VkCommandBuffer> m_commandBuffers;

        // Synchronization
        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::vector<VkFence> m_inFlightFences;

        // State
        uint32_t m_currentFrame;
        uint32_t m_imageIndex;
        SDL_Window* m_window;
        bool m_initialized = false;

        // Initialization methods
        bool CreateInstance();
        bool CreateSurface();
        bool SelectPhysicalDevice();
        bool CreateLogicalDevice();
        bool CreateSwapchain();
        bool CreateImageViews();
        bool CreateRenderPass();
        bool CreateFramebuffers();
        bool CreateCommandPool();
        bool CreateCommandBuffers();
        bool CreateSyncObjects();

        // Helper methods
        void CleanupSwapchain();

        // Handle Device Lost
        bool m_deviceLost = false;
        bool RecreateVulkanDevice();
    };

} // namespace REngine