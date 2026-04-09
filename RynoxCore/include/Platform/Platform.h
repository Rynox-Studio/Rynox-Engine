#pragma once

#include "IWindow.h"

#include <cstdint>
#include <string>

#include <Math/Vec2.h>

namespace Rynox::Platform
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

	//
	// Window
	//

	IWindow* CreateWindow();
	void DestroyWindow(IWindow* window);
}
