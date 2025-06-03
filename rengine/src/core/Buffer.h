#pragma once
#include <Buffer.h>
#include <vector>

namespace REngine {
    class Buffer {
    public:
        enum class Type {
            Vertex,
            Index,
            Constant,
            Structured
        };

        enum class Usage {
            Default,
            Dynamic,
            Immutable
        };

        // For vertex/index buffers
        Buffer(Diligent::IRenderDevice* device,
               const void* data,
               size_t elementSize,
               size_t elementCount,
               Type type,
               Usage usage = Usage::Default);

        // For constant buffers
        Buffer(Diligent::IRenderDevice* device,
               size_t size,
               Usage usage = Usage::Dynamic);

        Diligent::IBuffer* GetDiligentBuffer() const { return m_Buffer.RawPtr(); }
        size_t GetElementCount() const { return m_ElementCount; }
        size_t GetElementSize() const { return m_ElementSize; }

        // For dynamic buffers
        void Update(Diligent::IDeviceContext* context, const void* data, size_t dataSize);

    private:
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_Buffer;
        size_t m_ElementSize = 0;
        size_t m_ElementCount = 0;
        Type m_Type;
    };
}