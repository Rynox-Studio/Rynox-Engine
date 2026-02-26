#include "Core/Win32Window.h"

#include <Common/Logger.h>

#include "Core/Events/WindowEvents.h"
#include "Core/Events/KeyEvents.h"
#include "Core/Events/MouseEvents.h"

namespace Rynox::Core 
{
	static int s_WindowCount = 0;
	const wchar_t* WINDOW_CLASS_NAME = L"RynoxWindow";

	Win32Window::Win32Window()
	{
		Register();
	}

	Win32Window::~Win32Window()
	{
		if (m_Handle)
		{
			Unregister();
			DestroyWindow(reinterpret_cast<HWND>(m_Handle));
			m_Handle = nullptr;
		}
	}

	bool Win32Window::Initialize(const WindowDesc& desc)
	{
		std::wstring wtitle(desc.Title.begin(), desc.Title.end());

		m_Handle =
			::CreateWindowExW(
				0,
				WINDOW_CLASS_NAME,
				wtitle.c_str(),
				WS_OVERLAPPEDWINDOW,
				desc.Position[0],
				desc.Position[1],
				desc.Size[0],
				desc.Size[1],
				desc.Parent != nullptr ? reinterpret_cast<HWND>(desc.Parent->GetNativeHandle()) : NULL,
				NULL,
				GetModuleHandleW(NULL),
				&m_Data
			);
		if (!m_Handle)
		{
			// TODO: Add error logging
			return false;
		}

		m_Data.callback = desc.EventCallback;

		if (desc.Show)
		{
			ShowWindow(reinterpret_cast<HWND>(m_Handle), SW_SHOW);
			m_Data.shown = true;
		}
		if (desc.Focus)
		{
			SetFocus(m_Handle);
		}
		UpdateWindow(reinterpret_cast<HWND>(m_Handle));

		return true;
	}

	std::string Win32Window::GetTitle() const
	{
		if (!m_Handle) return "";

		int len = GetWindowTextLengthW(reinterpret_cast<HWND>(m_Handle));
		if (len > 0)
		{
			std::wstring wtitle;
			wtitle.resize(len + 1, '\0');

			GetWindowTextW(reinterpret_cast<HWND>(m_Handle), wtitle.data(), len + 1);

			std::string title(wtitle.begin(), wtitle.end());
			return title;
		}
		return "";
	}

	void Win32Window::GetPosition(int* x, int* y) const
	{
		if (!m_Handle) return;

		*x = m_Data.x;
		*y = m_Data.y;
	}

	void Win32Window::GetSize(int* width, int* height) const
	{
		if (!m_Handle) return;

		*width = m_Data.width;
		*height = m_Data.height;
	}

	void Win32Window::SetTitle(const std::string& title)
	{
		if (!m_Handle) return;

		std::wstring wtitle(title.begin(), title.end());
		SetWindowTextW(reinterpret_cast<HWND>(m_Handle), wtitle.c_str());
	}

	void Win32Window::SetPosition(int x, int y)
	{
		if (!m_Handle) return;

		SetWindowPos(reinterpret_cast<HWND>(m_Handle), 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	void Win32Window::SetSize(int width, int height)
	{
		if (!m_Handle) return;

		SetWindowPos(reinterpret_cast<HWND>(m_Handle), 0, 0, 0, width, height, SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	void Win32Window::SetEventCallback(std::function<void(IEvent&)> callback)
	{
		m_Data.callback = callback;
	}

	void Win32Window::PollEvents()
	{
		if (!m_Handle) return;

		MSG msg;
		while (PeekMessageW(&msg, reinterpret_cast<HWND>(m_Handle), 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	void* Win32Window::GetNativeHandle()
	{
		return m_Handle;
	}

	void Win32Window::Show()
	{
		if (!m_Handle) return;

		ShowWindow(reinterpret_cast<HWND>(m_Handle), SW_SHOW);
		m_Data.shown = true;
	}

	void Win32Window::Hide()
	{
		if (!m_Handle) return;

		ShowWindow(reinterpret_cast<HWND>(m_Handle), SW_HIDE);
		m_Data.shown = false;
	}

	bool Win32Window::IsShown()
	{
		if (!m_Handle) return false;

		return m_Data.shown;
	}

	bool Win32Window::ShouldClose()
	{
		if (!m_Handle) return true;

		return m_Data.should_close;
	}

	void Win32Window::Register()
	{
		if (s_WindowCount == 0)
		{
			WNDCLASSEXW wcx{ sizeof(wcx) };
			wcx.style = CS_OWNDC;
			wcx.lpfnWndProc = WndProc;
			wcx.cbClsExtra = 0;
			wcx.cbWndExtra = 0;
			wcx.hInstance = GetModuleHandleW(0);
			wcx.hIcon = 0;
			wcx.hCursor = LoadCursorW(nullptr, IDC_ARROW);
			wcx.hbrBackground = 0;
			wcx.lpszMenuName = 0;
			wcx.lpszClassName = WINDOW_CLASS_NAME;
			wcx.hIconSm = 0;

			RegisterClassExW(&wcx);
		}
		s_WindowCount++;
	}

	void Win32Window::Unregister()
	{
		s_WindowCount--;
		if (s_WindowCount == 0)
		{
			UnregisterClassW(WINDOW_CLASS_NAME, GetModuleHandleW(0));
		}
	}

	LRESULT WINAPI Win32Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WindowData* data = reinterpret_cast<WindowData*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

		switch (uMsg)
		{

		case WM_ERASEBKGND: return 1;


		case WM_NCCREATE:
		{
			CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
			data = static_cast<WindowData*>(cs->lpCreateParams);

			SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data));

			return DefWindowProcW(hWnd, uMsg, wParam, lParam);
		} break;

		case WM_CLOSE:
		{
			data->should_close = true;
			if (data->callback)
			{
				WindowCloseEvent e;
				data->callback(e);
			}
		} break;

		case WM_EXITSIZEMOVE:
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			if (rc.right != data->width || rc.bottom != data->height)
			{
				data->width = rc.right;
				data->height = rc.bottom;
				if (data->callback)
				{
					WindowResizeEvent e(rc.right, rc.bottom);
					data->callback(e);
				}
			}

			POINT p{ 0, 0 };
			ClientToScreen(hWnd, &p);
			if (p.x != data->x || p.y != data->y)
			{
				data->x = p.x;
				data->y = p.y;
				if (data->callback)
				{
					WindowMoveEvent e(p.x, p.y);
					data->callback(e);
				}
			}
		} break;

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
		{
			bool focused = (uMsg == WM_SETFOCUS);
			if (data->callback)
			{
				WindowFocusEvent e(focused);
				data->callback(e);
			}
		} break;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			if (data->callback)
			{
				bool isDown = (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN);
				int scancode = (lParam >> 16) & 0xFF;

				if (isDown)
				{
					bool isFirst = !(lParam & (1 << 30));
					if (isFirst) [[unlikely]]
					{
						KeyDownEvent e(scancode);
						data->callback(e);
					}
				}
				else
				{
					KeyUpEvent e(scancode);
					data->callback(e);
				}
			}
		} break;

		case WM_CHAR:
		case WM_SYSCHAR:
		{
			if (data->callback)
			{
				int scancode = (lParam >> 16) & 0xFF;
				int codepoint = wParam;

				KeyCharEvent e(scancode, codepoint);
				data->callback(e);
			}
		} break;

		case WM_MOUSEMOVE:
		{
			if (data->callback)
			{
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);
				int dx = x - data->mouse_last_x;
				int dy = y - data->mouse_last_y;

				MouseMoveEvent e(x, y, dx, dy);
				data->callback(e);
			}
		} break;

		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
		{
			if (data->callback)
			{
				bool isHorizontal = (uMsg == WM_MOUSEHWHEEL);
				float delta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;

				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);

				if (isHorizontal) [[unlikely]]
				{
					MouseScrollEvent e(x, y, delta, 0);
					data->callback(e);
				}
				else [[likely]]
				{
					MouseScrollEvent e(x, y, 0, delta);
					data->callback(e);
				}
			}
		} break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		{
			if (data->callback)
			{
				int button;
				bool isDown = !(uMsg % 2);
				int x = GET_X_LPARAM(lParam);
				int y = GET_Y_LPARAM(lParam);

				if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)
					button = 0;
				else if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP)
					button = 1;
				else if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)
					button = 2;
				else if (uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONUP)
				{
					button = 2 + HIWORD(wParam);
				}

				if (isDown)
				{
					MouseButtonDownEvent e(x, y, button);
					data->callback(e);
				}
				else
				{
					MouseButtonUpEvent e(x, y, button);
					data->callback(e);
				}
			}

		} break;

		default: return DefWindowProcW(hWnd, uMsg, wParam, lParam);

		}

		return 0;
	}
}