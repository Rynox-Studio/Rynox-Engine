#include "Core/IWindow.h"

#include <Common/Platform.h>

#ifdef RNX_PLATFORM_WINDOWS
#	include "Core/Win32Window.h"
#endif

namespace Rynox::Core
{
	std::unique_ptr<IWindow> IWindow::Create()
	{
#ifdef RNX_PLATFORM_WINDOWS
		return std::make_unique<Win32Window>();
#endif
	}
}
