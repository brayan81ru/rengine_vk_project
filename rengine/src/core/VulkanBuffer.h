#pragma once
#include <vulkan/vulkan.h>
namespace REngine {
    class VulkanBuffer {
    public:
        VkBuffer GetBuffer() const { return m_buffer; }
        VkDeviceMemory GetMemory() const { return m_memory; }

        void Create(
            VkDevice device,
            VkPhysicalDevice physicalDevice,
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties
        );

        void Destroy();

    private:
        VkDevice m_device = VK_NULL_HANDLE;
        VkBuffer m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;
    };
}