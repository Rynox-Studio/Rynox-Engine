#include "Core/IWindow.h"

#include <Common/Platform.h>

#ifdef RNX_PLATFORM_WINDOWS
#	include "Core/Win32Window.h"
#endif

namespace Rynox::Core
{
	IWindow* IWindow::Create()
	{
#ifdef RNX_PLATFORM_WINDOWS
		return new Win32Window();
#endif
	}
}
