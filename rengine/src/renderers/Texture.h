#pragma once
#include <vulkan/vulkan.h>
#include <glm.hpp>
#include <string>
#include <memory>

namespace REngine {
    class Texture {
    public:
        Texture();
        ~Texture();

        // Disable copying
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // Creation methods
        void CreateFromFile(
            VkDevice device,
            VkPhysicalDevice physicalDevice,
            VkCommandPool commandPool,
            VkQueue queue,
            const std::string& path,
            VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
            bool generateMipmaps = true
        );

        void CreateFromData(
            VkDevice device,
            VkPhysicalDevice physicalDevice,
            VkCommandPool commandPool,
            VkQueue queue,
            const void* pixels,
            uint32_t width,
            uint32_t height,
            VkFormat format,
            bool generateMipmaps = true
        );

        // Bindless support
        void SetBindlessIndex(uint32_t index) { m_bindlessIndex = index; }
        uint32_t GetBindlessIndex() const { return m_bindlessIndex; }

        // Vulkan resources
        VkImage GetImage() const { return m_image; }
        VkImageView GetView() const { return m_imageView; }
        VkSampler GetSampler() const { return m_sampler; }

        // Dimensions
        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }
        VkFormat GetFormat() const { return m_format; }

    private:
        void CreateSampler();
        void GenerateMipmaps(VkCommandBuffer cmd, VkPhysicalDevice physicalDevice);
        void TransitionImageLayout(VkCommandBuffer cmd, VkImageLayout oldLayout, VkImageLayout newLayout);
        void CopyBufferToImage(VkCommandBuffer cmd, VkBuffer buffer);

        // Vulkan resources
        VkDevice m_device = VK_NULL_HANDLE;
        VkImage m_image = VK_NULL_HANDLE;
        VkImageView m_imageView = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;
        VkSampler m_sampler = VK_NULL_HANDLE;

        // Texture properties
        uint32_t m_width = 0;
        uint32_t m_height = 0;
        uint32_t m_mipLevels = 1;
        VkFormat m_format = VK_FORMAT_UNDEFINED;

        // Bindless support
        uint32_t m_bindlessIndex = UINT32_MAX;
    };
}