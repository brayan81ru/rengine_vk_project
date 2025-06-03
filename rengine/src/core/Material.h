#pragma once
#include <PipelineState.h>
#include <ShaderResourceBinding.h>
#include <core/Shader.h>
#include <core/Texture.h>
#include <unordered_map>
#include <memory>
#include "RenderDevice.h"

namespace REngine {
    class Material {
    public:
        Material(Diligent::IRenderDevice* device,
                 std::shared_ptr<Shader> vertexShader,
                 std::shared_ptr<Shader> pixelShader);

        void SetTexture(const std::string& name, std::shared_ptr<Texture> texture);
        void SetConstant(const std::string& name, const void* data, size_t size);

        void Apply(Diligent::IDeviceContext* context);

        Diligent::IPipelineState* GetPipelineState() const { return m_PipelineState.RawPtr(); }

    private:
        void CreatePipelineState(Diligent::IRenderDevice* device);
        void CreateShaderBindings(Diligent::IRenderDevice* device);

        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_PipelineState;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_ShaderBindings;

        std::shared_ptr<Shader> m_VertexShader;
        std::shared_ptr<Shader> m_PixelShader;

        std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
        std::unordered_map<std::string, std::vector<uint8_t>> m_Constants;
    };
}
