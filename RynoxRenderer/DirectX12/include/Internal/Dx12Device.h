#pragma once

#include <DirectX12.h>

namespace Rynox::DirectX12
{
	struct Dx12Device
	{
		ComPtr<IDXGIFactory7> Factory;
		ComPtr<IDXGIAdapter4> Adapter;
		ComPtr<ID3D12Device14> Device;

		bool Initialize();
		void Shutdown();

		ID3D12Device14* operator->() const { return Device.Get(); }

		operator IDXGIFactory7* () const { return Factory.Get(); }
		operator IDXGIAdapter4* () const { return Adapter.Get(); }
		operator ID3D12Device14* () const { return Device.Get(); }
	};
}
