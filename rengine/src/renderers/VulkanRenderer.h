#pragma once
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>

namespace REngine {

    class VulkanRenderer {
    public:
        VulkanRenderer();
        ~VulkanRenderer();

        // Initialize Vulkan on an SDL window
        bool Initialize(SDL_Window* window);

        // Cleanup resources
        void Shutdown();

        // Begin/End frame
        void BeginFrame();
        void EndFrame();

    private:
        // Vulkan core components
        VkInstance m_instance;
        VkPhysicalDevice m_physicalDevice;
        VkDevice m_device;
        VkQueue m_graphicsQueue;
        VkSurfaceKHR m_surface;

        // SDL window reference
        SDL_Window* m_window;

        // Debug messenger for validation layers
        VkDebugUtilsMessengerEXT m_debugMessenger;

        // Private initialization methods
        bool CreateInstance();
        bool CreateSurface();
        bool SelectPhysicalDevice();
        bool CreateLogicalDevice();
        bool SetupDebugMessenger();

        // Helper functions
        bool CheckValidationLayerSupport(const std::vector<const char *> &validationLayers);
        std::vector<const char*> GetRequiredExtensions();
    };

} // namespace REngine