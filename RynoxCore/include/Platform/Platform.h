#pragma once

#include <cstdint>
#include <string>

#include <Math/Vec2.h>
#include "IWindow.h"

namespace Rynox::Core::Platform
{
	//
	// System Info
	//

	uint32_t GetLogicalCoreCount();
	uint64_t GetTotalMemory();
	std::string GetOSVersion();

	//
	// Dynamic Library
	//

	void* LoadLibrary(std::string_view path);
	void* GetSymbol(void* library, std::string_view name);
	void UnloadLibrary(void* library);

	//
	// Cursor
	//

	void SetCursorPosition(Math::Vec2 position);
	Math::Vec2 GetCursorPosition();
	void SetCursorVisible(bool visible);
	void SetCaptureCursor(bool capture);

	//
	// Window
	//

	IWindow* CreateWindow();
	void DestroyWindow(IWindow* window);
}
