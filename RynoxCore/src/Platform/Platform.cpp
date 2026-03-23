#include <Platform/Platform.h>

#include <Common/Platform.h>
#include <Common/Assert.h>

#ifdef RNX_WINDOWS
#	include <Windows.h>
#	include <Platform/Win32Window.h>
#	undef CreateWindow
#	undef LoadLibrary
#else
#	error "Unsupported OS"
#endif

namespace
{
#ifdef RNX_WINDOWS
	const SYSTEM_INFO g_SystemInfo = []() -> SYSTEM_INFO
		{
			SYSTEM_INFO info;
			GetSystemInfo(&info);
			return info;
		}();

	const MEMORYSTATUSEX g_MemStatus = []() -> MEMORYSTATUSEX
		{
			MEMORYSTATUSEX status;
			status.dwLength = sizeof(status);
			GlobalMemoryStatusEx(&status);
			return status;
		}();
#endif
}

namespace Rynox::Platform
{
	uint32_t GetLogicalCoreCount()
	{
#ifdef RNX_WINDOWS
		return static_cast<uint32_t>(g_SystemInfo.dwNumberOfProcessors);
#endif
	}

	uint64_t GetTotalMemory()
	{
#ifdef RNX_WINDOWS
		return static_cast<uint64_t>(g_MemStatus.ullTotalPhys);
#endif
	}

	std::string GetOSVersion()
	{
#ifdef RNX_WINDOWS
		OSVERSIONINFOEXW info{};
		info.dwOSVersionInfoSize = sizeof(info);

		using RtlGetVersionFn = NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW);
		const auto RtlGetVersion = reinterpret_cast<RtlGetVersionFn>(
			GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion")
			);
		RNX_ASSERT(RtlGetVersion);

		if (RtlGetVersion)
		{
			RtlGetVersion(reinterpret_cast<PRTL_OSVERSIONINFOW>(&info));
		}

		return std::to_string(info.dwMajorVersion) + "." +
			std::to_string(info.dwMinorVersion) + "." +
			std::to_string(info.dwBuildNumber);
#endif
	}

	void* LoadLibrary(std::string_view path)
	{
#ifdef RNX_WINDOWS
		return static_cast<void*>(::LoadLibraryA(path.data()));
#endif
	}

	void* GetSymbol(void* library, std::string_view name)
	{
#ifdef RNX_WINDOWS
		return reinterpret_cast<void*>(::GetProcAddress(static_cast<HMODULE>(library), name.data()));
#endif
	}

	void UnloadLibrary(void* library)
	{
#ifdef RNX_WINDOWS
		::FreeLibrary(static_cast<HMODULE>(library));
#endif
	}

	void SetCursorPosition(Math::Vec2 position)
	{
#ifdef RNX_WINDOWS
		::SetCursorPos(static_cast<int>(position.x), static_cast<int>(position.y));
#endif
	}

	Math::Vec2 GetCursorPosition()
	{
#ifdef RNX_WINDOWS
		POINT p;
		::GetCursorPos(&p);
		return { static_cast<float>(p.x), static_cast<float>(p.y) };
#endif
	}

	void SetCursorVisible(bool visible)
	{
#ifdef RNX_WINDOWS
		::ShowCursor(visible ? TRUE : FALSE);
#endif
	}

	void SetCaptureCursor(bool capture)
	{
#ifdef RNX_WINDOWS
		if (capture)
		{
			RECT rect;
			::GetClipCursor(&rect);
			::ClipCursor(&rect);
		}
		else
		{
			::ClipCursor(nullptr);
		}
#endif
	}

	IWindow* CreateWindow()
	{
		IWindow* window = nullptr;
#ifdef RNX_WINDOWS
		window = new Win32Window();
#endif
		return window;
	}

	void DestroyWindow(IWindow* window)
	{
		delete window;
	}
}