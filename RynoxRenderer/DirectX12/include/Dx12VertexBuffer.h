#pragma once

#include <DirectX12.h>

namespace Rynox::DirectX12
{
    class Dx12VertexBuffer
    {
    public:
        Dx12VertexBuffer() = default;
        Dx12VertexBuffer(
            ID3D12Device14* device,
            ID3D12GraphicsCommandList* cmdList,
            const void* data,
            UINT size,
            UINT stride);

        ~Dx12VertexBuffer() = default;

        Dx12VertexBuffer(const Dx12VertexBuffer&) = delete;
        Dx12VertexBuffer& operator=(const Dx12VertexBuffer&) = delete;

        Dx12VertexBuffer(Dx12VertexBuffer&&) = default;
        Dx12VertexBuffer& operator=(Dx12VertexBuffer&&) = default;

        void Bind(ID3D12GraphicsCommandList* cmdList, UINT slot = 0) const;

        // Call this after the command list has been executed and the fence
        // has signaled — frees the staging buffer
        void ReleaseStagingBuffer();

        bool IsValid() const { return m_Resource != nullptr; }

    private:
        ComPtr<ID3D12Resource> m_Resource;
        ComPtr<ID3D12Resource> m_StagingBuffer;
        D3D12_VERTEX_BUFFER_VIEW m_View = {};
    };
}