#pragma once
#include <Shader.h>
#include <RefCntAutoPtr.hpp>
#include <string>

namespace REngine {
    class Shader {
    public:
        enum class Type {
            Vertex,
            Pixel,
            Geometry,
            Compute
        };

        Shader(Diligent::IRenderDevice* device,
               const std::string& filePath,
               Type type,
               const std::string& entryPoint = "main");

        Diligent::IShader* GetDiligentShader() const { return m_Shader.RawPtr(); }
        Type GetShaderType() const { return m_Type; }

        // Reload support
        void Reload(Diligent::IRenderDevice* device);

    private:
        Diligent::RefCntAutoPtr<Diligent::IShader> m_Shader;
        Type m_Type;
        std::string m_FilePath;
        std::string m_EntryPoint;
    };
}