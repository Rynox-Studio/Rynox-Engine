#pragma once

#include <Common/Macros.h>
#include <Common/Assert.h>
#include <Core/Graphics/IRenderer.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")

#include <wrl/client.h>

namespace Rynox::DirectX12
{
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	constexpr UINT BUFFER_COUNT = 2;

	static DXGI_FORMAT ToDXGI(Graphics::ShaderDataType type)
	{
		using enum Graphics::ShaderDataType;

		switch (type)
		{
		case Float: return DXGI_FORMAT_R32_FLOAT;
		case Float2: return DXGI_FORMAT_R32G32_FLOAT;
		case Mat3:
		case Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case Mat4:
		case Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;

		case Int: return DXGI_FORMAT_R32_SINT;
		case Int2: return DXGI_FORMAT_R32G32_SINT;
		case Int3: return DXGI_FORMAT_R32G32B32_SINT;
		case Int4: return DXGI_FORMAT_R32G32B32A32_SINT;

		case Bool: return DXGI_FORMAT_R32_UINT;
		}

		RNX_ASSERT(false, "Unknown ShaderDataType");
		return DXGI_FORMAT_UNKNOWN;
	}
}
