#include <Dx12IndexBuffer.h>

#include <Common/Assert.h>

namespace Rynox::DirectX12
{
    Dx12IndexBuffer::Dx12IndexBuffer(
        ID3D12Device14* device,
        ID3D12GraphicsCommandList* cmdList,
        const void* data,
        UINT                       size,
        DXGI_FORMAT                format)
    {
        RNX_ASSERT(device, "Device is null");
        RNX_ASSERT(cmdList, "CommandList is null");
        RNX_ASSERT(data, "Data is null");
        RNX_ASSERT(size, "Size is 0");
        RNX_ASSERT(format == DXGI_FORMAT_R16_UINT || format == DXGI_FORMAT_R32_UINT);

        const UINT indexSize = (format == DXGI_FORMAT_R16_UINT) ? 2 : 4;
        m_IndexCount = size / indexSize;

        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        desc.Alignment = 0;
        desc.Width = size;
        desc.Height = 1;
        desc.DepthOrArraySize = 1;
        desc.MipLevels = 1;
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        desc.Flags = D3D12_RESOURCE_FLAG_NONE;

        D3D12_HEAP_PROPERTIES uploadHeap = {};
        uploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;
        uploadHeap.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        uploadHeap.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        uploadHeap.CreationNodeMask = 1;
        uploadHeap.VisibleNodeMask = 1;

        HRESULT hr = device->CreateCommittedResource(
            &uploadHeap,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(m_StagingBuffer.ReleaseAndGetAddressOf())
        );
        if (FAILED(hr))
        {
            RNX_LOG_ERROR("[DirectX12] Failed to create Staging Buffer.");
        }

        D3D12_HEAP_PROPERTIES defaultHeap = {};
        defaultHeap.Type = D3D12_HEAP_TYPE_DEFAULT;
        defaultHeap.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        defaultHeap.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        defaultHeap.CreationNodeMask = 1;
        defaultHeap.VisibleNodeMask = 1;

        hr = device->CreateCommittedResource(
            &defaultHeap,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(m_Resource.ReleaseAndGetAddressOf())
        );
        if (FAILED(hr))
        {
            RNX_LOG_ERROR("[DirectX12] Failed to create Index Buffer.");
            return;
        }

        void* mapped = nullptr;
        D3D12_RANGE range{ 0, 0 };
        m_StagingBuffer->Map(0, &range, &mapped);
        memcpy(mapped, data, size);
        m_StagingBuffer->Unmap(0, nullptr);

        cmdList->CopyBufferRegion(
            m_Resource.Get(), 0,
            m_StagingBuffer.Get(), 0,
            size
        );

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = m_Resource.Get();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        cmdList->ResourceBarrier(1, &barrier);

        m_View.BufferLocation = m_Resource->GetGPUVirtualAddress();
        m_View.SizeInBytes = size;
        m_View.Format = format;
    }

    void Dx12IndexBuffer::Bind(ID3D12GraphicsCommandList* cmdList) const
    {
        RNX_ASSERT(cmdList && IsValid());
        cmdList->IASetIndexBuffer(&m_View);
    }

    void Dx12IndexBuffer::ReleaseStagingBuffer()
    {
        m_StagingBuffer.Reset();
    }

}