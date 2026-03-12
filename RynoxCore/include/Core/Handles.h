#pragma once

#include <cstdint>

namespace Rynox
{
	template<typename T>
	struct GPUHandle
	{
	public:
		uint32_t index = UINT32_MAX;
		uint32_t generation = UINT32_MAX;

		GPUHandle() = default;
		GPUHandle(uint32_t i, uint32_t g) : index(i), generation(g) {}

		bool IsValid() const
		{
			return index != UINT32_MAX && generation != UINT32_MAX;
		}
	};
}