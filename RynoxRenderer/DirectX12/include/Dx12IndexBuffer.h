#pragma once

#include <DirectX12.h>

namespace Rynox::DirectX12
{
    class Dx12IndexBuffer
    {
    public:
        Dx12IndexBuffer() = default;
        Dx12IndexBuffer(
            ID3D12Device14* device,
            ID3D12GraphicsCommandList* cmdList,
            const void* data,
            UINT size,
            DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);

        ~Dx12IndexBuffer() = default;

        Dx12IndexBuffer(const Dx12IndexBuffer&) = delete;
        Dx12IndexBuffer& operator=(const Dx12IndexBuffer&) = delete;

        Dx12IndexBuffer(Dx12IndexBuffer&&) = default;
        Dx12IndexBuffer& operator=(Dx12IndexBuffer&&) = default;

        void Bind(ID3D12GraphicsCommandList* cmdList) const;
        void ReleaseStagingBuffer();

        UINT GetIndexCount() const { return m_IndexCount; }
        bool IsValid() const { return m_Resource != nullptr; }

    private:
        ComPtr<ID3D12Resource> m_Resource;
        ComPtr<ID3D12Resource> m_StagingBuffer;
        D3D12_INDEX_BUFFER_VIEW m_View = {};
        UINT m_IndexCount = 0;
    };
}