#pragma once

#include <Common/Macros.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <wrl/client.h>

namespace Rynox::Renderer
{
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	constexpr UINT BUFFER_COUNT = 2;
}
