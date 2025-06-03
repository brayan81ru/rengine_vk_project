#pragma once
#include <Texture.h>
#include <string>
#include <memory>
namespace REngine {
    class Texture {
    public:
        enum class Type {
            Texture2D,
            TextureCube,
            TextureArray
        };

        Texture(Diligent::IRenderDevice* device,
                const std::string& path,
                Type type = Type::Texture2D,
                bool generateMips = true);

        // Immediate loading
        static std::shared_ptr<Texture> CreateImmediate(
            Diligent::IRenderDevice* device,
            const std::string& path,
            Type type = Type::Texture2D);

        // Async loading
        static void LoadAsync(Diligent::IRenderDevice* device,
                             const std::string& path,
                             std::function<void(std::shared_ptr<Texture>)> callback);

        Diligent::ITexture* GetDiligentTexture() const { return m_Texture.RawPtr(); }
        Diligent::ITextureView* GetShaderView() const { return m_ShaderView.RawPtr(); }

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        Type GetType() const { return m_Type; }

    private:
        Diligent::RefCntAutoPtr<Diligent::ITexture> m_Texture;
        Diligent::RefCntAutoPtr<Diligent::ITextureView> m_ShaderView;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        Type m_Type;
    };
}