#include <Renderer/Dx12VertexBuffer.h>

#include <Common/Assert.h>

namespace Rynox::Renderer
{

    Dx12VertexBuffer::Dx12VertexBuffer(
        ID3D12Device14* device,
        ID3D12GraphicsCommandList* cmdList,
        const void* data,
        UINT                       size,
        UINT                       stride)
    {
        RNX_ASSERT(device, "Device is null");
        RNX_ASSERT(cmdList, "CommandList is null");
        RNX_ASSERT(data, "Data is null");
        RNX_ASSERT(size, "Size is 0");
        RNX_ASSERT(stride, "Stride is 0");

        // ------------------------------------------------------------------
        // Shared buffer desc — same shape for both buffers, only heap differs
        // ------------------------------------------------------------------
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

        // ------------------------------------------------------------------
        // 1. Staging buffer — UPLOAD heap, CPU writes into this
        // ------------------------------------------------------------------
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
            return;
        }

        // ------------------------------------------------------------------
        // 2. Final buffer — DEFAULT heap, GPU-only VRAM
        // ------------------------------------------------------------------
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
            D3D12_RESOURCE_STATE_COPY_DEST, // ready to receive the copy
            nullptr,
            IID_PPV_ARGS(m_Resource.ReleaseAndGetAddressOf())
        );
        if (FAILED(hr))
        {
            RNX_LOG_ERROR("[DirectX12] Failed to create Vertex Buffer.");
            return;
        }

        // ------------------------------------------------------------------
        // 3. CPU writes into staging
        // ------------------------------------------------------------------
        void* mapped = nullptr;
        D3D12_RANGE range{ 0, 0 };
        m_StagingBuffer->Map(0, &range, &mapped);
        memcpy(mapped, data, size);
        m_StagingBuffer->Unmap(0, nullptr);

        // ------------------------------------------------------------------
        // 4. Record: GPU copies staging → final
        // ------------------------------------------------------------------
        cmdList->CopyBufferRegion(
            m_Resource.Get(), 0,   // dst, dst offset
            m_StagingBuffer.Get(), 0,   // src, src offset
            size
        );

        // ------------------------------------------------------------------
        // 5. Transition final buffer → vertex buffer readable
        // ------------------------------------------------------------------
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = m_Resource.Get();
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        cmdList->ResourceBarrier(1, &barrier);

        // ------------------------------------------------------------------
        // 6. Build the view — used every Bind() call
        // ------------------------------------------------------------------
        m_View.BufferLocation = m_Resource->GetGPUVirtualAddress();
        m_View.SizeInBytes = size;
        m_View.StrideInBytes = stride;
    }

    void Dx12VertexBuffer::Bind(ID3D12GraphicsCommandList* cmdList, UINT slot) const
    {
        RNX_ASSERT(cmdList && IsValid());
        cmdList->IASetVertexBuffers(slot, 1, &m_View);
    }

    void Dx12VertexBuffer::ReleaseStagingBuffer()
    {
        m_StagingBuffer.Reset();
    }

}