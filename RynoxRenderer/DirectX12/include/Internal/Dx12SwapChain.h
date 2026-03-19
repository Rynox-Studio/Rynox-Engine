#pragma once

#include <DirectX12.h>

namespace Rynox::DirectX12
{
	struct Dx12SwapChain
	{
		ComPtr<IDXGISwapChain4> SwapChain;
		ComPtr<ID3D12DescriptorHeap> RTVHeap;
		ComPtr<ID3D12Resource> RTVs[BUFFER_COUNT];
		UINT RTVDescSize = 0;

		ComPtr<ID3D12DescriptorHeap> DSVHeap;
		ComPtr<ID3D12Resource> DepthStencilBuffer;

		UINT FrameIndex = 0;

		bool Initialize(
			IDXGIFactory7* factory,
			ID3D12Device14* device,
			ID3D12CommandQueue* queue,
			HWND hWnd,
			uint32_t width, uint32_t height);

		bool Resize(ID3D12Device14* device, uint32_t width, uint32_t height);
		void Shutdown();

		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const;
		ID3D12Resource* GetCurrentBackBuffer() const;

	private:
		bool CreateSizeDependentResources(ID3D12Device14* device, uint32_t width, uint32_t height);
	};
}
