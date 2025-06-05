#include "Shader.h"
#include <fstream>
#include <stdexcept>
#include <algorithm>
namespace REngine {
    Shader::Shader(VkDevice device) : m_device(device) {}

    Shader::~Shader() {
        for (auto& [stage, module] : m_modules) {
            vkDestroyShaderModule(m_device, module, nullptr);
        }
        if (m_layout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(m_device, m_layout, nullptr);
        }
    }

    void Shader::CreateShaderModule(const std::vector<uint32_t>& code, Stage stage) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size() * sizeof(uint32_t);
        createInfo.pCode = code.data();

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module!");
        }
        m_modules[stage] = shaderModule;
    }

    void Shader::ReflectDescriptors(const std::vector<uint32_t>& code) {
        SpvReflectShaderModule module;
        if (spvReflectCreateShaderModule(code.size() * sizeof(uint32_t), code.data(), &module) != SPV_REFLECT_RESULT_SUCCESS) {
            throw std::runtime_error("Failed to reflect SPIR-V!");
        }

        uint32_t count = 0;
        spvReflectEnumerateDescriptorSets(&module, &count, nullptr);
        std::vector<SpvReflectDescriptorSet*> sets(count);
        spvReflectEnumerateDescriptorSets(&module, &count, sets.data());

        for (const auto& set : sets) {
            for (uint32_t i = 0; i < set->binding_count; ++i) {
                const auto& binding = *set->bindings[i];
                m_bindings.push_back({
                    set->set,  // Set index
                    binding.binding,
                    static_cast<VkDescriptorType>(binding.descriptor_type),
                    static_cast<VkShaderStageFlags>(module.shader_stage)
                });
            }
        }

        spvReflectDestroyShaderModule(&module);
    }

    void Shader::BuildPipelineLayout() {
        std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> setBindings;

        for (const auto& binding : m_bindings) {
            setBindings[binding.set].push_back({
                binding.binding,
                binding.type,
                1,  // descriptorCount
                binding.stageFlags,
                nullptr  // pImmutableSamplers
            });
        }

        // Create descriptor set layouts
        m_setLayouts.reserve(setBindings.size());
        for (const auto& [set, bindings] : setBindings) {
            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings = bindings.data();

            VkDescriptorSetLayout layout;
            if (vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &layout) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create descriptor set layout!");
            }
            m_setLayouts.push_back(layout);
        }

        // Create pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(m_setLayouts.size());
        pipelineLayoutInfo.pSetLayouts = m_setLayouts.data();

        if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_layout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout!");
        }
    }

    void Shader::LoadFromFile(const std::string& path, Stage stage) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open shader file: " + path);
        }

        const size_t fileSize = file.tellg();
        std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
        file.seekg(0);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        file.close();

        CreateShaderModule(buffer, stage);
        ReflectDescriptors(buffer);
    }

    void Shader::Reload() {
        // Implementation for hot-reloading would go here
        // Would need to track file paths per stage
    }
}