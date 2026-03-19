#include <Internal/Dx12Device.h>

namespace Rynox::DirectX12
{
	bool Dx12Device::Initialize()
	{
		HRESULT hr = S_OK;

		{
			UINT Flags = 0;
#ifndef NDEBUG
			Flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

			hr = CreateDXGIFactory2(Flags, IID_PPV_ARGS(&Factory));
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create DXGI Factory.");
				return false;
			}
		}

		{
			ComPtr<IDXGIAdapter1> adapter;

			for (UINT i = 0;
				Factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
				i++)
			{
				DXGI_ADAPTER_DESC1 aDesc;
				adapter->GetDesc1(&aDesc);

				// Skip software adapters
				if (aDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

				hr = D3D12CreateDevice(
					adapter.Get(),
					D3D_FEATURE_LEVEL_12_0,
					__uuidof(ID3D12Device), nullptr
				);
				if (SUCCEEDED(hr))
				{
					adapter.As(&Adapter);
					break;
				}
			}

			if (!Adapter)
			{
				RNX_LOG_ERROR("[DirectX12] Failed to select DXGI Adapter.");
				return false;
			}
		}

		{
			hr = D3D12CreateDevice(
				Adapter.Get(),
				D3D_FEATURE_LEVEL_12_0,
				IID_PPV_ARGS(&Device)
			);
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create D3D12 Device.");
				return false;
			}
		}

		return true;
	}

	void Dx12Device::Shutdown()
	{
		Device.Reset();
		Adapter.Reset();
		Factory.Reset();
	}
}
