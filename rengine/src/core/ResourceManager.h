#pragma once
#include <core/Texture.h>
#include <core/Shader.h>
#include <core/Material.h>
#include <unordered_map>
#include <memory>

namespace REngine {
    class ResourceManager {
    public:
        static ResourceManager& Get();

        // Texture management
        std::shared_ptr<Texture> LoadTexture(
            Diligent::IRenderDevice* device,
            const std::string& path,
            Texture::Type type = Texture::Type::Texture2D);

        // Shader management
        std::shared_ptr<Shader> LoadShader(
            Diligent::IRenderDevice* device,
            const std::string& path,
            Shader::Type type);

        // Material management
        std::shared_ptr<Material> CreateMaterial(
            Diligent::IRenderDevice* device,
            const std::string& vertexShaderPath,
            const std::string& pixelShaderPath);

        // Resource cleanup
        void ReleaseUnusedResources();

    private:
        ResourceManager() = default;

        std::unordered_map<std::string, std::weak_ptr<Texture>> m_TextureCache;
        std::unordered_map<std::string, std::weak_ptr<Shader>> m_ShaderCache;
    };
}
