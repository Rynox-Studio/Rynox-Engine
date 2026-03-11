#pragma once

#include <cstdint>
#include <atomic>

namespace Rynox::ECS
{
	struct TypeID
	{
		using size_type = uint32_t;

		template<typename T>
		static size_type Get()
		{
			static size_type index =
			return index;
		}
	private:
		inline static std::atomic<size_type> s_Counter = 0;
	};
}
