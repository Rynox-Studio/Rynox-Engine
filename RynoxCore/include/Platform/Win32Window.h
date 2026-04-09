// This file should be included ONLY in "IWindow.h"
#pragma once

#define UNICODE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#include "IWindow.h"

namespace Rynox
{
	class Win32Window final : public IWindow 
	{
	public:
		Win32Window();
		virtual ~Win32Window() override;

		bool Initialize(const WindowDesc& desc) override;

		void SetTitle(std::string_view title) override;
		void SetPosition(Math::Vec2 position) override;
		void SetSize(Math::Vec2 size) override;
		void SetMinSize(Math::Vec2 minSize) override;
		void SetMaxSize(Math::Vec2 maxSize) override;
		void SetEventCallback(std::function<void(IEvent&)> callback) override;
		void SetCursorType(CursorType type) override;

		void SetResizable(bool resizable) override;
		void SetMinimizable(bool minimizable) override;
		void SetMaximizable(bool maximizable) override;
		void SetVisible(bool visible) override;
		void SetBorderless(bool borderless) override;
		void SetAlwaysOnTop(bool enable) override;
		void SetCaptureMouse(bool capture) override;

		void* GetNativeWindow() override;
		std::string GetTitle() const override;
		Math::Vec2 GetPosition() const override;
		Math::Vec2 GetSize() const override;
		Math::Vec2 GetMinSize() const override;
		Math::Vec2 GetMaxSize() const override;
		WindowFlags GetFlags() const override;
		CursorType GetCursorType() const override;

		void PollEvents() override;

		void Minimize() override;
		void Maximize() override;
		void Restore() override;

		double GetContentScale() const override;

	private:
		static std::wstring GetWinError();

		static void Register();
		static void Unregister();
		static HCURSOR GetCursor(CursorType type);
		static LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		HWND m_Handle = nullptr;
		
		using Events = uint32_t;
		struct Event
		{
			enum : Events
			{
				None = 0,

				Move = RNX_BIT(0),
				Size = RNX_BIT(1),
				Maximize = RNX_BIT(2),
				Minimize = RNX_BIT(3),
				MouseMove = RNX_BIT(4),
				MouseWheel = RNX_BIT(5),
			};
		};

		struct WindowData 
		{
			std::wstring title;
			Math::Vec2 position;
			Math::Vec2 size;
			Math::Vec2 max_size;
			Math::Vec2 min_size;
			
			CursorType cursor_type;
			HCURSOR cursor = nullptr;

			Math::Vec2 mouse_pos;
			Math::Vec2 delta_mouse_pos;
			Math::Vec2 mouse_wheel;

			Events events;
			WindowFlags flags;

			std::function<void(IEvent&)> callback = nullptr;
		} m_Data;
	};
}
