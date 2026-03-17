#pragma once

#include <cstdint>

namespace Rynox::Graphics
{
	constexpr uint32_t GPU_INVALID_HANDLE = UINT32_MAX;

	template<typename T>
	struct GPUHandle
	{
	public:
		uint32_t id = GPU_INVALID_HANDLE;
		uint32_t gen = GPU_INVALID_HANDLE;

		GPUHandle() = default;
		GPUHandle(uint32_t i, uint32_t g) : id(i), gen(g) {}
	};

	struct MeshTag {};
	struct ShaderTag {};

	using MeshHandle = GPUHandle<MeshTag>;
	using ShaderHandle = GPUHandle<ShaderTag>;
}