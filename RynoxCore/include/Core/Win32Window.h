// This file must not be included in header files!
#pragma once

#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#include "IWindow.h"

namespace Rynox::Core 
{
	class Win32Window : public IWindow 
	{
	public:
		Win32Window();
		virtual ~Win32Window() override;

		bool Initialize(const WindowDesc& desc) override;

		std::string GetTitle() const override;
		void GetPosition(int* x, int* y) const override;
		void GetSize(int* width, int* height) const override;

		void SetTitle(const std::string& title) override;
		void SetPosition(int x, int y) override;
		void SetSize(int width, int height) override;
		void SetEventCallback(std::function<void(IEvent&)> callback) override;

		void PollEvents() override;
		void* GetNativeHandle() override;

		void Show() override;
		void Hide() override;
		bool IsShown() override;

		bool ShouldClose() override;

	private:
		static void Register();
		static void Unregister();

		static LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		HWND m_Handle = nullptr;

		struct WindowData 
		{
			int x = 0, y = 0;
			int width = 0, height = 0;

			int mouse_last_x = 0, mouse_last_y = 0;
			bool shown = false;
			bool should_close = false;
			std::function<void(IEvent&)> callback = nullptr;
		};
		WindowData m_Data;
	};
}
