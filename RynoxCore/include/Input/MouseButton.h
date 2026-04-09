#pragma once

#include <cstdint>

// Mouse buttons based on USB HID Usage Tables (https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf)
// This provides a consistent set across platforms, abstracting away platform-specific differences.

namespace Rynox
{
	using MouseButton = uint32_t;
	namespace Mouse
	{
		enum : MouseButton
		{
			None = 0,

			Button1,
			Button2,
			Button3,
			Button4,
			Button5,

			Count,
			
			// Aliases
			
			LeftButton = Button1,
			RightButton = Button2,
			MiddleButton = Button3,
		};
	}
}