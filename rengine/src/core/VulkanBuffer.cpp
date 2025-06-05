#include "VulkanBuffer.h"
#include <VulkanHelpers.h>
namespace REngine {
    void VulkanBuffer::Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties
    ) {
        m_device = device;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, m_buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(
            physicalDevice,
            memRequirements.memoryTypeBits,
            properties
        );

        if (vkAllocateMemory(device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device, m_buffer, m_memory, 0);
    }

    void VulkanBuffer::Destroy() {
        if (m_device) {
            vkDestroyBuffer(m_device, m_buffer, nullptr);
            vkFreeMemory(m_device, m_memory, nullptr);
            m_buffer = VK_NULL_HANDLE;
            m_memory = VK_NULL_HANDLE;
        }
    }
}