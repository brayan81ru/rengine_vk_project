#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <spirv_reflect.h>

namespace REngine {
    class Shader {
    public:
        enum Stage {
            VERTEX = 0,
            FRAGMENT,
            COMPUTE,
            RAYGEN,
            CLOSEST_HIT,
            MISS,
            ANY_HIT,
            INTERSECTION,
            STAGE_COUNT
        };

        Shader(VkDevice device);
        ~Shader();

        void LoadFromFile(const std::string& path, Stage stage);
        void BuildPipelineLayout();
        void Reload();

        VkPipelineLayout GetLayout() const { return m_layout; }
        VkShaderModule GetModule(Stage stage) const {
            const auto it = m_modules.find(stage);
            return (it != m_modules.end()) ? it->second : VK_NULL_HANDLE;
        }

        bool IsRayTracingShader() const {
            return m_modules.count(RAYGEN) ||
                   m_modules.count(CLOSEST_HIT) ||
                   m_modules.count(MISS);
        }

    private:
        void CreateShaderModule(const std::vector<uint32_t>& code, Stage stage);
        void ReflectDescriptors(const std::vector<uint32_t>& code);

        VkDevice m_device;
        std::unordered_map<Stage, VkShaderModule> m_modules;
        VkPipelineLayout m_layout = VK_NULL_HANDLE;

        struct DescriptorBinding {
            uint32_t set;  // This was missing in original
            uint32_t binding;
            VkDescriptorType type;
            VkShaderStageFlags stageFlags;
        };
        std::vector<VkDescriptorSetLayout> m_setLayouts;
        std::vector<DescriptorBinding> m_bindings;
    };
}