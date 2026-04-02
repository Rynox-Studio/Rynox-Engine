#pragma once

#include <cstdint>

namespace Rynox
{
	using MouseCode = uint32_t;
	namespace Mouse
	{
		enum : MouseCode
		{
			None = 0,

			LeftButton = 1,
			RightButton = 2,
			MiddleButton = 3,

			Button4 = 4,
			Button5 = 5,
			Button6 = 6,
			Button7 = 7,
			Button8 = 8,

			Count
		};
	}
}