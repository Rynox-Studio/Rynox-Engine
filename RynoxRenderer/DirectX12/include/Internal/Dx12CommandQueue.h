#pragma once

#include <DirectX12.h>

namespace Rynox::DirectX12
{
	struct Dx12CommandQueue
	{
		ComPtr<ID3D12CommandQueue> Queue;
		ComPtr<ID3D12Fence> Fence;
		HANDLE FenceEvent = nullptr;
		UINT64 FenceCounter = 0;

		bool Initialize(ID3D12Device14* device);
		void Shutdown();

		UINT64 Signal();
		void WaitForValue(UINT64 value);
		void Flush();
		void Execute(ID3D12GraphicsCommandList* list);

		operator ID3D12CommandQueue* () const { return Queue.Get(); }
		operator ID3D12Fence* () const { return Fence.Get(); }
	};
}
