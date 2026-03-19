#pragma once

#include <DirectX12.h>

namespace Rynox::DirectX12
{
    struct Dx12CommandList
    {
        ComPtr<ID3D12CommandAllocator> Allocators[BUFFER_COUNT];
        ComPtr<ID3D12GraphicsCommandList> List;

        bool Initialize(ID3D12Device14* device);
        void Shutdown();

        void Reset(uint32_t frameIndex);

        ID3D12GraphicsCommandList* operator->() const { return List.Get(); }

        operator ID3D12GraphicsCommandList* () const { return List.Get(); }
    };
}
