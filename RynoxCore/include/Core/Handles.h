#pragma once

#include <cstdint>

namespace Rynox
{
	template<typename T>
	struct GPUHandle
	{
	public:
		uint32_t index = UINT32_MAX;

		GPUHandle() = default;
		GPUHandle(uint32_t i) : index(i) {}

		bool IsValid() const
		{
			return index != UINT32_MAX;
		}
	};
}