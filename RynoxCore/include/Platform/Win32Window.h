// This file must not be included in header files!
#pragma once

#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#include "IWindow.h"

namespace Rynox
{
	class Win32Window : public IWindow 
	{
	public:
		Win32Window();
		virtual ~Win32Window() override;

		bool Initialize(const WindowDesc& desc) override;

		std::string GetTitle() const override;
		Math::Vec2 GetPosition() const override;
		Math::Vec2 GetSize() const override;

		void SetTitle(std::string_view title) override;
		void SetPosition(Math::Vec2 position) override;
		void SetSize(Math::Vec2 size) override;
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
