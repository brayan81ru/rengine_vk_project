#include "Shader.h"
#include "Core/TextureManager.h"
#include "DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "DiligentCore/Graphics/GraphicsTools/interface/MapHelper.hpp"
#include <fstream>
#include <sstream>

namespace REngine {
    Shader::Shader() {
    }

    Shader::~Shader() {
        // Automatic cleanup via RefCntAutoPtr
    }

    bool Shader::Load(const std::string& vertexPath, const std::string& pixelPath) {
        std::string vertexSrc, pixelSrc;

        if (!ReadShaderFile(vertexPath, vertexSrc)) return false;
        if (!ReadShaderFile(pixelPath, pixelSrc)) return false;

        return LoadFromMemory(vertexSrc, pixelSrc);
    }

    bool Shader::LoadFromMemory(const std::string& vertexSrc, const std::string& pixelSrc) {
        auto* pDevice = TextureManager::GetInstance().GetDevice();

        // Compile shaders
        Diligent::RefCntAutoPtr<Diligent::IShader> pVS;
        if (!CompileShader(vertexSrc, Diligent::SHADER_TYPE_VERTEX, &pVS)) {
            return false;
        }

        Diligent::RefCntAutoPtr<Diligent::IShader> pPS;
        if (!CompileShader(pixelSrc, Diligent::SHADER_TYPE_PIXEL, &pPS)) {
            return false;
        }

        // Create pipeline state using GraphicsPipelineStateCreateInfo
        Diligent::GraphicsPipelineStateCreateInfo PSOCreateInfo;
        PSOCreateInfo.PSODesc.Name = "Shader PSO";
        PSOCreateInfo.PSODesc.PipelineType = Diligent::PIPELINE_TYPE_GRAPHICS;

        // Set shaders
        PSOCreateInfo.pVS = pVS;
        PSOCreateInfo.pPS = pPS;

        // Setup render targets
        PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
        PSOCreateInfo.GraphicsPipeline.RTVFormats[0] = Diligent::TEX_FORMAT_RGBA8_UNORM;
        PSOCreateInfo.GraphicsPipeline.DSVFormat = Diligent::TEX_FORMAT_D32_FLOAT;

        // Default states
        PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = Diligent::CULL_MODE_BACK;

        // Create vertex buffer layout
        Diligent::LayoutElement LayoutElems[] = {
            {0, 0, 3, Diligent::VT_FLOAT32, false}, // Position
            {1, 0, 2, Diligent::VT_FLOAT32, false}  // UV
        };
        PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
        PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(LayoutElems);

        // Create uniform buffer
        Diligent::BufferDesc CBDesc;
        CBDesc.Name = "VS constants CB";
        CBDesc.Size = sizeof(Diligent::float4x4);
        CBDesc.Usage = Diligent::USAGE_DYNAMIC;
        CBDesc.BindFlags = Diligent::BIND_UNIFORM_BUFFER;
        CBDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;
        pDevice->CreateBuffer(CBDesc, nullptr, &m_VSConstants);

        // Create PSO
        pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_PSO);
        if (!m_PSO) return false;

        // Bind uniform buffer
        m_PSO->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "Constants")->Set(m_VSConstants);

        // Create shader resource binding
        m_PSO->CreateShaderResourceBinding(&m_SRB, true);

        return true;
    }

    void Shader::Bind() {
        auto* pCtx = TextureManager::GetInstance().GetContext();
        pCtx->SetPipelineState(m_PSO);
        pCtx->CommitShaderResources(m_SRB, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }

    void Shader::SetUniform(const std::string& name, const Matrix4x4& value) {
        if (name == "Constants") {
            Diligent::MapHelper<Diligent::float4x4> CBConstants(
                TextureManager::GetInstance().GetContext(),
                m_VSConstants,
                Diligent::MAP_WRITE,
                Diligent::MAP_FLAG_DISCARD
            );
            *CBConstants = *reinterpret_cast<const Diligent::float4x4*>(&value);
        }
    }

    void Shader::SetTexture(const std::string& name, const Texture& texture) {
        if (!m_SRB) return;

        // Get non-const texture view
        Diligent::ITextureView* pTextureView = const_cast<Texture&>(texture).GetDiligentTextureView();
        if (!pTextureView) return;

        auto* pVar = m_SRB->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, name.c_str());
        if (pVar) {
            pVar->Set(pTextureView);
        }
    }

    bool Shader::CompileShader(const std::string& source, Diligent::SHADER_TYPE type, Diligent::IShader** ppShader) {
        Diligent::ShaderCreateInfo shaderCI;
        shaderCI.SourceLanguage = Diligent::SHADER_SOURCE_LANGUAGE_HLSL;
        shaderCI.Desc.ShaderType = type;
        shaderCI.Desc.Name = type == Diligent::SHADER_TYPE_VERTEX ? "Vertex shader" : "Pixel shader";
        shaderCI.Source = source.c_str();
        shaderCI.EntryPoint = "main";

        auto* pDevice = TextureManager::GetInstance().GetDevice();
        pDevice->CreateShader(shaderCI, ppShader);

        return *ppShader != nullptr;
    }

    bool Shader::ReadShaderFile(const std::string& path, std::string& outSource) {
        std::ifstream file(path);
        if (!file.is_open()) return false;

        std::stringstream buffer;
        buffer << file.rdbuf();
        outSource = buffer.str();
        return true;
    }
} // namespace REngine