#include "Platform/Win32Window.h"


#include "Event/WindowEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
#include <Common/Logger.h>

#include <hidusage.h>

namespace Rynox
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
			DestroyWindow(m_Handle);
			m_Handle = nullptr;
		}
	}
	
	bool Win32Window::Initialize(const WindowDesc& desc)
	{
		m_Data.title = std::wstring(desc.Title.begin(), desc.Title.end());
		m_Data.position = desc.Position;
		m_Data.size = desc.Size;
		m_Data.max_size = desc.MaxSize;
		m_Data.min_size = desc.MinSize;
		m_Data.cursor_type = desc.CursorType;

		DWORD style = WS_OVERLAPPEDWINDOW;
		DWORD exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

		RECT rc = { 0, 0, (LONG)m_Data.size.x, (LONG)m_Data.size.y };
		AdjustWindowRectEx(&rc, style, FALSE, exStyle);
		m_Data.size = { float(rc.right - rc.left), float(rc.bottom - rc.top) };
		
		m_Handle =
			::CreateWindowExW(
				exStyle,
				WINDOW_CLASS_NAME,
				m_Data.title.c_str(),
				style,
				m_Data.position.x, m_Data.position.y,
				m_Data.size.x, m_Data.size.y,
				NULL,
				NULL,
				NULL,
				&m_Data
			);
		if (!m_Handle)
		{
#ifdef _DEBUG
			std::wstring werror = GetWinError();
			std::string error(werror.begin(), werror.end());
			RNX_LOG_ERROR("[Win32] {}", error.c_str());
#endif
			return false;
		}

		m_Data.flags = desc.Flags;
		if (m_Data.flags & WindowFlag::Minimized && m_Data.flags & WindowFlag::Maximized)
		{
			m_Data.flags &= ~(WindowFlag::Minimized | WindowFlag::Maximized);
		}

		SetCursorType(m_Data.cursor_type);
		SetResizable(m_Data.flags & WindowFlag::Resizable);
		SetMinimizable(m_Data.flags & WindowFlag::Minimizable);
		SetMaximizable(m_Data.flags & WindowFlag::Maximizable);
		SetVisible(m_Data.flags & WindowFlag::Visible);
		SetBorderless(m_Data.flags & WindowFlag::Borderless);
		SetAlwaysOnTop(m_Data.flags & WindowFlag::AlwaysOnTop);
		SetCaptureMouse(m_Data.flags & WindowFlag::CaptureMouse);

		if (m_Data.flags & WindowFlag::Minimized) Minimize();
		else if (m_Data.flags & WindowFlag::Maximized) Maximize();

		POINT p;
		GetCursorPos(&p);
		ScreenToClient(m_Handle, &p);
		m_Data.mouse_pos = { float(p.x), float(p.y) };
		
		RAWINPUTDEVICE devices[2];
		devices[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		devices[0].usUsage = HID_USAGE_GENERIC_KEYBOARD;
		devices[0].dwFlags = 0;
		devices[0].hwndTarget = m_Handle;

		devices[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
		devices[1].usUsage = HID_USAGE_GENERIC_MOUSE;
		devices[1].dwFlags = 0;
		devices[1].hwndTarget = m_Handle;

		if (RegisterRawInputDevices(devices, _countof(devices), sizeof(devices[0])) == FALSE)
		{
#ifdef _DEBUG
			std::wstring werror = GetWinError();
			std::string error(werror.begin(), werror.end());
			RNX_LOG_ERROR("[Win32] {}", error.c_str());
#endif
			return false;
		}

		m_Data.callback = desc.EventCallback;
		UpdateWindow(m_Handle);

		return true;
	}

	void Win32Window::SetTitle(std::string_view title)
	{
		if (!m_Handle) return;
		std::wstring wtitle(title.begin(), title.end());
		SetWindowTextW(m_Handle, wtitle.c_str());
	}

	void Win32Window::SetPosition(Math::Vec2 position)
	{
		if (!m_Handle) return;
		SetWindowPos(m_Handle, 0, position.x, position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	void Win32Window::SetSize(Math::Vec2 size)
	{
		if (!m_Handle) return;
		SetWindowPos(m_Handle, 0, 0, 0, size.x, size.y, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}
	
	void Win32Window::SetMinSize(Math::Vec2 minSize)
	{
		m_Data.min_size = minSize;
	}

    void Win32Window::SetMaxSize(Math::Vec2 maxSize)
    {
		m_Data.max_size = maxSize;
    }

    void Win32Window::SetEventCallback(std::function<void(IEvent &)> callback)
    {
		m_Data.callback = callback;
	}

    void Win32Window::SetCursorType(CursorType type)
    {
		m_Data.cursor_type = type;
		m_Data.cursor = GetCursor(type);
		SetCursor(m_Data.cursor);
    }

    void Win32Window::SetResizable(bool resizable)
    {
		if (!m_Handle) return;
		LONG_PTR style = GetWindowLongPtrW(m_Handle, GWL_STYLE);
		resizable ? style |= WS_THICKFRAME : style &= ~WS_THICKFRAME;
		SetWindowLongPtrW(m_Handle, GWL_STYLE, style);
		SetWindowPos(m_Handle, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    	m_Data.flags = (m_Data.flags & ~WindowFlag::Resizable) | (-(int)resizable & WindowFlag::Resizable);
    }

    void Win32Window::SetMinimizable(bool minimizable)
    {
		if (!m_Handle) return;
		LONG_PTR style = GetWindowLongPtrW(m_Handle, GWL_STYLE);
		minimizable ? style |= WS_MINIMIZEBOX : style &= ~WS_MINIMIZEBOX;
		SetWindowLongPtrW(m_Handle, GWL_STYLE, style);
		m_Data.flags = (m_Data.flags & ~WindowFlag::Minimizable) | (-(int)minimizable & WindowFlag::Minimizable);
    }

    void Win32Window::SetMaximizable(bool maximizable)
    {
		if (!m_Handle) return;
		LONG_PTR style = GetWindowLongPtrW(m_Handle, GWL_STYLE);
		maximizable ? style |= WS_MAXIMIZEBOX : style &= ~WS_MAXIMIZEBOX;
		SetWindowLongPtrW(m_Handle, GWL_STYLE, style);
		m_Data.flags = (m_Data.flags & ~WindowFlag::Maximizable) | (-(int)maximizable & WindowFlag::Maximizable);
    }

    void Win32Window::SetVisible(bool visible)
    {
		if (!m_Handle) return;
		LONG_PTR style = GetWindowLongPtrW(m_Handle, GWL_STYLE);
		visible ? style |= WS_VISIBLE : style &= ~WS_VISIBLE;
		SetWindowLongPtrW(m_Handle, GWL_STYLE, style);
		m_Data.flags = (m_Data.flags & ~WindowFlag::Visible) | (-(int)visible & WindowFlag::Visible);
    }

	void Win32Window::SetBorderless(bool borderless)
	{
	    if (!m_Handle) return;
	    LONG_PTR style = GetWindowLongPtrW(m_Handle, GWL_STYLE);
	    borderless ? style &= ~WS_CAPTION : style |= WS_CAPTION;
	    SetWindowLongPtrW(m_Handle, GWL_STYLE, style);
	    SetWindowPos(m_Handle, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	    m_Data.flags = (m_Data.flags & ~WindowFlag::Borderless) | (-(int)borderless & WindowFlag::Borderless);
	}

    void Win32Window::SetAlwaysOnTop(bool enable)
    {
		if (!m_Handle) return;
		SetWindowPos(m_Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		m_Data.flags = (m_Data.flags & ~WindowFlag::AlwaysOnTop) | (-(int)enable & WindowFlag::AlwaysOnTop);
    }

    void Win32Window::SetCaptureMouse(bool capture)
    {
		if (!m_Handle || m_Data.flags & WindowFlag::CaptureMouse) return;
		if (capture) SetCapture(m_Handle); else ReleaseCapture(); // cond ? on-true : on-false failed to work here IDK
		m_Data.flags = (m_Data.flags & ~WindowFlag::CaptureMouse) | (-(int)capture & WindowFlag::CaptureMouse);
    }

    void* Win32Window::GetNativeWindow()
    {
        return static_cast<void*>(m_Handle);
    }

    std::string Win32Window::GetTitle() const
    {
		std::string title(m_Data.title.begin(), m_Data.title.end());
		return title;
	}

	Math::Vec2 Win32Window::GetPosition() const
	{
		return m_Data.position;
	}

	Math::Vec2 Win32Window::GetSize() const
	{
		return m_Data.size;
	}

    Math::Vec2 Win32Window::GetMinSize() const
    {
        return m_Data.min_size;
    }

    Math::Vec2 Win32Window::GetMaxSize() const
    {
        return m_Data.max_size;
    }

    WindowFlags Win32Window::GetFlags() const
    {
        return m_Data.flags;
    }

    CursorType Win32Window::GetCursorType() const
    {
        return m_Data.cursor_type;
    }

    void Win32Window::PollEvents()
    {
		if (!m_Handle) return;

		MSG msg;
		while (PeekMessageW(&msg, m_Handle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		if (m_Data.events & Event::Move)
		{
			POINT p = {};
			ClientToScreen(m_Handle, &p);
			m_Data.position.x = p.x;
			m_Data.position.y = p.y;

			WindowMoveEvent e(m_Data.position);
			if (m_Data.callback)
			{
				m_Data.callback(e);
			}
		}

		if (m_Data.events & Event::Size)
		{
			RECT rc;
			GetClientRect(m_Handle, &rc);
			m_Data.size.x = rc.right - rc.left;
			m_Data.size.y = rc.bottom - rc.top;

			WindowResizeEvent e(m_Data.size);
			if (m_Data.callback)
			{
				m_Data.callback(e);
			}
		}

		if (m_Data.events & Event::MouseMove)
		{
			POINT p;
			GetCursorPos(&p);
			ScreenToClient(m_Handle, &p);

			m_Data.mouse_pos.x = p.x;
			m_Data.mouse_pos.y = p.y;

			if (m_Data.callback)
			{
				MouseMoveEvent e(m_Data.mouse_pos, m_Data.delta_mouse_pos);
				m_Data.callback(e);
			}
		}

		if (m_Data.events & Event::MouseWheel && m_Data.callback)
		{
			MouseScrollEvent e(m_Data.mouse_pos, m_Data.mouse_wheel);
			m_Data.callback(e);

			m_Data.mouse_wheel = { 0, 0 };
		}

		m_Data.delta_mouse_pos = {};
		m_Data.events = None;
	}

    void Win32Window::Minimize()
    {
		if (m_Handle != nullptr) ShowWindow(m_Handle, SW_MINIMIZE);
    }

    void Win32Window::Maximize()
    {
		if (m_Handle != nullptr) ShowWindow(m_Handle, SW_MAXIMIZE);
    }

    void Win32Window::Restore()
    {
		if (m_Handle != nullptr) ShowWindow(m_Handle, SW_RESTORE);
    }

    double Win32Window::GetContentScale() const
    {
		if (!m_Handle) return 1.0;
        UINT dpi = GetDpiForWindow(m_Handle);
		return static_cast<double>(dpi) / 96.0;
    }

    std::wstring Win32Window::GetWinError()
    {
		DWORD err = GetLastError();
		if (err)
		{
			wchar_t* buffer = nullptr;
			FormatMessageW(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
        		FORMAT_MESSAGE_FROM_SYSTEM |
        		FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,
				err,
				0,
				buffer,
				0,
				nullptr
			);

			std::wstring result = buffer ? buffer : L"Unknown Error";

			if (buffer) LocalFree(buffer);
			return result;
		}
		return L"No error";
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

    HCURSOR Win32Window::GetCursor(CursorType type)
    {
		switch (type)
		{
			case CursorType::None: return NULL;
			case CursorType::Arrow: return LoadCursorW(NULL, IDC_ARROW);
			case CursorType::Ibeam: return LoadCursorW(NULL, IDC_IBEAM);
			case CursorType::Wait: return LoadCursorW(NULL, IDC_WAIT);
			case CursorType::Crosshair: return LoadCursorW(NULL, IDC_CROSS);
			case CursorType::ResizeVertical: return LoadCursorW(NULL, IDC_SIZENS);
			case CursorType::ResizeHorizontal: return LoadCursorW(NULL, IDC_SIZEWE);
			case CursorType::ResizeLR: return LoadCursorW(NULL, IDC_SIZENWSE);
			case CursorType::ResizeRL: return LoadCursorW(NULL, IDC_SIZENESW);
			case CursorType::SizeAll: return LoadCursorW(NULL, IDC_SIZEALL);
			case CursorType::NotAllowed: return LoadCursorW(NULL, IDC_NO);
			case CursorType::Help: return LoadCursorW(NULL, IDC_HELP);
		}
		return NULL;
    }

    LRESULT WINAPI Win32Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WindowData* data = reinterpret_cast<WindowData*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
		if (!data && uMsg != WM_NCCREATE) return DefWindowProcW(hWnd, uMsg, wParam, lParam);

		switch (uMsg)
		{

			case WM_ERASEBKGND: return 1;

			case WM_SETCURSOR:
			{
				if (LOWORD(lParam) == HTCLIENT)
				{
					SetCursor(data->cursor);
					return TRUE;
				}
			} return DefWindowProcW(hWnd, uMsg, wParam, lParam);

			case WM_GETMINMAXINFO:
			{
				MINMAXINFO* mmi = (MINMAXINFO*)lParam;

				mmi->ptMinTrackSize.x = data->min_size.x;
				mmi->ptMinTrackSize.y = data->min_size.y;

				mmi->ptMaxTrackSize.x = data->max_size.x;
				mmi->ptMaxTrackSize.y = data->max_size.y;
			} break;

			case WM_NCCREATE:
			{
				CREATESTRUCTW* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
				data = static_cast<WindowData*>(cs->lpCreateParams);

				SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data));

				return DefWindowProcW(hWnd, uMsg, wParam, lParam);
			} break;

			case WM_CLOSE:
			{
				if (data->callback)
				{
					WindowCloseEvent e;
					data->callback(e);
				}
			} break;

			case WM_EXITSIZEMOVE:
			{
				POINT p{ 0, 0 };
				ClientToScreen(hWnd, &p);
				if (p.x != data->position.x || p.y != data->position.y)
				{
					data->events |= Event::Move;
				}
				
				RECT rc;
				GetClientRect(hWnd, &rc);
				if (rc.right != data->size.x || rc.bottom != data->size.y)
				{
					data->events |= Event::Size;
				}
			} break;

			case WM_MOVE:
			{
				int x = LOWORD(lParam);
				int y = HIWORD(lParam);

				if (data->position.x != x || data->position.y != y)
				{
					data->events |= Event::Move;
				}
			} break;

			case WM_SIZE:
			{
				int width = LOWORD(lParam);
				int height = HIWORD(lParam);

				if (data->size.x != width || data->size.y != height)
				{
					data->events |= Event::Size;
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

			case WM_INPUT:
			{
				UINT size = 0;
				GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

				alignas(8) uint8_t stack_buf[256];
				void* buf = (size <= sizeof(stack_buf)) ? stack_buf : malloc(size);

				if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buf, &size, sizeof(RAWINPUTHEADER)) == (UINT)-1)
				{
					if (buf != stack_buf) free(buf);
					break;
				}

				RAWINPUT* raw = (RAWINPUT*)buf;

				switch (raw->header.dwType)
				{
					case RIM_TYPEKEYBOARD:
					{
						const RAWKEYBOARD& kb = raw->data.keyboard;

						bool is_down = !(kb.Flags & RI_KEY_BREAK);
						bool e0 = (kb.Flags & RI_KEY_E0);

						KeyCode key = Key::None;

						if (key != Key::None && data->callback)
						{
							if (is_down)
							{
								KeyDownEvent e(key);
								data->callback(e);
							}
							else
							{
								KeyUpEvent e(key);
								data->callback(e);
							}
						}
					} break;

					case RIM_TYPEMOUSE:
					{
						const RAWMOUSE& mouse = raw->data.mouse;

						// Move
						if (mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
						{
							bool vd = mouse.usFlags & MOUSE_VIRTUAL_DESKTOP;
							int width = GetSystemMetrics(vd ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);
							int height = GetSystemMetrics(vd ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);

							float absX = (float)mouse.lLastX / 65535.0f * width;
    						float absY = (float)mouse.lLastY / 65535.0f * height;

							data->delta_mouse_pos.x = (float)mouse.lLastX - data->mouse_pos.x;
							data->delta_mouse_pos.y = (float)mouse.lLastY - data->mouse_pos.y;
						}
						else [[likely]]
						{
							data->delta_mouse_pos.x += (float)mouse.lLastX;
							data->delta_mouse_pos.y += (float)mouse.lLastY;
						}

						if (mouse.lLastX != 0 || mouse.lLastY != 0)
						{
							data->events |= Event::MouseMove;
						}

						// Wheel
						if (mouse.usButtonFlags & RI_MOUSE_WHEEL) 
						{
							data->mouse_wheel.y += (float)(SHORT)mouse.usButtonData / WHEEL_DELTA;
							data->events |= Event::MouseWheel;
						}
						else if (mouse.usButtonFlags & RI_MOUSE_HWHEEL) 
						{
							data->mouse_wheel.x += (float)(SHORT)mouse.usButtonData / WHEEL_DELTA;
							data->events |= Event::MouseWheel;
						}

						// Button
						static const struct { USHORT down; USHORT up; MouseButton button; } k_MouseButtons[] =
						{
							{ RI_MOUSE_BUTTON_1_DOWN, RI_MOUSE_BUTTON_1_UP, Mouse::Button1 },
        	    			{ RI_MOUSE_BUTTON_2_DOWN, RI_MOUSE_BUTTON_2_UP, Mouse::Button2 },
        	    			{ RI_MOUSE_BUTTON_3_DOWN, RI_MOUSE_BUTTON_3_UP, Mouse::Button3 },
        	    			{ RI_MOUSE_BUTTON_4_DOWN, RI_MOUSE_BUTTON_4_UP, Mouse::Button4 },
        	    			{ RI_MOUSE_BUTTON_5_DOWN, RI_MOUSE_BUTTON_5_UP, Mouse::Button5 },
						};

						for (auto& btn : k_MouseButtons)
						{
							if (mouse.usButtonFlags & btn.down)
        	    			{
        	    			    MouseButtonDownEvent e(data->mouse_pos, btn.button);
        	    			    data->callback(e);
        	    			}
        	    			if (mouse.usButtonFlags & btn.up)
        	    			{
        	    			    MouseButtonUpEvent e(data->mouse_pos, btn.button);
        	    			    data->callback(e);
        	    			}
						}
					} break;
				} // switch (raw->header.dwType)

				if (buf != stack_buf) free(buf);
			} return DefWindowProcW(hWnd, uMsg, wParam, lParam); // WM_INPUT

			default: return DefWindowProcW(hWnd, uMsg, wParam, lParam);
		} // switch (uMsg)

		return 0;
	}
}