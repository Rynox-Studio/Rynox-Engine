#pragma once

#include <DirectX12.h>
#include <Internal/Dx12CommandQueue.h>

namespace Rynox::DirectX12
{
	struct Dx12UploadContext
	{
        ComPtr<ID3D12CommandAllocator> Allocator;
        ComPtr<ID3D12GraphicsCommandList> List;
        bool Pending = false;

        bool Initialize(ID3D12Device14* device);
        void Shutdown();

        void Flush(Dx12CommandQueue& sync);
        void Reset();
	};
}
