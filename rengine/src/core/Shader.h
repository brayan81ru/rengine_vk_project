#pragma once
#include "DiligentCore/Graphics/GraphicsEngine/interface/PipelineState.h"
#include "DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceBinding.h"
#include "DiligentCore/Common/interface/RefCntAutoPtr.hpp"
#include "Core/Color.h"
#include "Core/Matrix4x4.h"
#include "Core/Texture.h"
#include <string>

namespace REngine {
    class Shader {
    public:
        Shader();
        ~Shader();

        bool Load(const std::string& vertexPath, const std::string& pixelPath);
        bool LoadFromMemory(const std::string& vertexSrc, const std::string& pixelSrc);

        void Bind();

        void SetUniform(const std::string& name, const Matrix4x4& value);
        void SetTexture(const std::string& name, const Texture& texture);

        Diligent::IPipelineState* GetPipelineState() { return m_PSO.RawPtr(); }
        Diligent::IShaderResourceBinding* GetResourceBinding() { return m_SRB.RawPtr(); }

    private:
        bool ReadShaderFile(const std::string& path, std::string& outSource);
        bool CompileShader(const std::string& source, Diligent::SHADER_TYPE type, Diligent::IShader** ppShader);

        Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_PSO;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_SRB;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_VSConstants;
    };
} // namespace REngine