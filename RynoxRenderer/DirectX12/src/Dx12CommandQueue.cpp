#include <Internal/Dx12CommandQueue.h>

namespace Rynox::DirectX12
{
	bool Dx12CommandQueue::Initialize(ID3D12Device14* device)
	{
		HRESULT hr = S_OK;

		{
			D3D12_COMMAND_QUEUE_DESC desc{};
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;

			hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&Queue));
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create D3D12 Command Queue.");
				return false;
			}
		}

		{
			FenceEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
			if (!FenceEvent)
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create Fence Event.");
				return false;
			}

			hr = device->CreateFence(
				0,
				D3D12_FENCE_FLAG_NONE,
				IID_PPV_ARGS(&Fence)
			);
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create D3D12 Fence.");
				return false;
			}
		}

		return true;
	}

	void Dx12CommandQueue::Shutdown()
	{
		Flush();

		if (FenceEvent)
		{
			CloseHandle(FenceEvent);
			FenceEvent = nullptr;
		}

		Fence.Reset();
		Queue.Reset();
	}

	UINT64 Dx12CommandQueue::Signal()
	{
		UINT64 value = ++FenceCounter;
		Queue->Signal(Fence.Get(), value);
		return value;
	}

	void Dx12CommandQueue::WaitForValue(UINT64 value)
	{
		if (Fence->GetCompletedValue() < value)
		{
			Fence->SetEventOnCompletion(value, FenceEvent);
			WaitForSingleObject(FenceEvent, INFINITE);
		}
	}

	void Dx12CommandQueue::Flush()
	{
		WaitForValue(Signal());
	}

	void Dx12CommandQueue::Execute(ID3D12GraphicsCommandList* list)
	{
		list->Close();
		ID3D12CommandList* lists[] = { list };
		Queue->ExecuteCommandLists(1, lists);
	}
}
