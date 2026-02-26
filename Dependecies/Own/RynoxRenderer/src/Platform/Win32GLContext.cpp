#include "Renderer/Platform/Win32GLContext.h"

#include <Common/Logger.h>

namespace Rynox::Renderer::Platform
{
    HMODULE Win32GLContext::s_opengl32 = nullptr;
    std::atomic<int> Win32GLContext::s_opengl32RefCount{ 0 };

	Win32GLContext::Win32GLContext(HWND hwnd)
	{
		m_hWnd = hwnd;
	}

	Win32GLContext::~Win32GLContext()
	{
		if (m_hRC)
		{
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(m_hRC);
			m_hRC = nullptr;
		}

		if (m_hDC && m_hWnd)
		{
			ReleaseDC(m_hWnd, m_hDC);
			m_hDC = nullptr;
		}

		if (m_tempHDC && m_tempHWnd)
		{
			ReleaseDC(m_tempHWnd, m_tempHDC);
			DestroyWindow(m_tempHWnd);
			m_tempHDC = nullptr;
			m_tempHWnd = nullptr;
		}

		if (Win32GLContext::s_opengl32RefCount.fetch_sub(1) == 1)
		{
			if (Win32GLContext::s_opengl32)
			{
				FreeLibrary(Win32GLContext::s_opengl32);
				Win32GLContext::s_opengl32 = nullptr;
			}
		}
	}

	bool Win32GLContext::Init()
	{
		if (!m_hWnd)
		{
			RNX_LOG_ERROR("HWND is NULL");
			return false;
		}

		m_hDC = GetDC(m_hWnd);
		if (!m_hDC)
		{
			RNX_LOG_ERROR("HDC is NULL");
			return false;
		}

		if (Win32GLContext::s_opengl32RefCount.fetch_add(1) == 0)
		{
			Win32GLContext::s_opengl32 = LoadLibraryA("opengl32.dll");
			if (!Win32GLContext::s_opengl32)
			{
				RNX_LOG_ERROR("Failed to load opengl32.dll");
				Win32GLContext::s_opengl32RefCount.fetch_sub(1);
				return false;
			}
		}

		m_tempHRC = CreateTempContext();
		if (!m_tempHRC)
		{
			RNX_LOG_ERROR("error create temp context");
			return false;
		}

		if (!wglMakeCurrent(m_tempHDC, m_tempHRC))
		{
			RNX_LOG_ERROR("error make temp context current");
			ReleaseTempContext();
			return false;
		}

		if (!LoadWGLFunctions())
		{
			RNX_LOG_ERROR("error load WGL functions");
			ReleaseTempContext();
			return false;
		}

		int pixelFormat = ChooseModernPixelFormat();
		if (pixelFormat == 0)
		{
			RNX_LOG_ERROR("error choose modern pixel format");
			ReleaseTempContext();
			return false;
		}

		ReleaseTempContext();

		if (!SetModernPixelFormat(pixelFormat))
		{
			RNX_LOG_ERROR("error choose modern pixel format");
			ReleaseTempContext();
			return false;
		}

		m_hRC = CreateModernContext();
		if (!m_hRC)
		{
			RNX_LOG_ERROR("error create modern context");
			return false;
		}

		if (!wglMakeCurrent(m_hDC, m_hRC))
		{
			RNX_LOG_ERROR("error make modern context current");
			return false;
		}

		return true;
	}

	void Win32GLContext::SwapBuffers()
	{
		::SwapBuffers(m_hDC);
	}

	bool Win32GLContext::MakeCurrent()
	{
		return wglMakeCurrent(m_hDC, m_hRC);
	}

	void* Win32GLContext::GetOpenGLLoader()
	{
		return (void*)&Win32GLContext::LoadProc;
	}

	HGLRC Win32GLContext::CreateTempContext()
	{
		if (!CreateTempWindowAndHDC())
			return nullptr;

		PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW |
			PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			32,
			0, 0, 0, 0, 0, 0,
			8, 0,
			0, 0, 0, 0, 0,
			24,
			8,
			0,
			PFD_MAIN_PLANE,
			0, 0, 0, 0
		};

		int pixelFormat = ChoosePixelFormat(m_tempHDC, &pfd);
		if (!pixelFormat) {
			RNX_LOG_ERROR("error choose pixel format");
			ReleaseTempContext();
			return nullptr;
		}

		if (!SetPixelFormat(m_tempHDC, pixelFormat, &pfd)) {
			RNX_LOG_ERROR("error set pixel format");
			ReleaseTempContext();
			return nullptr;
		}

		return wglCreateContext(m_tempHDC);
	}

	bool Win32GLContext::LoadWGLFunctions()
	{
		wglCreateContextAttribsARB =
			(PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

		if (!wglCreateContextAttribsARB) {
			RNX_LOG_ERROR("error load wglCreateContextAttribsARB");
			return false;
		}

		wglChoosePixelFormatARB =
			(PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

		if (!wglChoosePixelFormatARB) {
			RNX_LOG_ERROR("error load wglChoosePixelFormatARB");
			return false;
		}

		wglSwapIntervalEXT =
			(PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

		if (!wglSwapIntervalEXT) {
			RNX_LOG_ERROR("error load wglSwapIntervalEXT");
			return false;
		}

		return true;
	}

	int Win32GLContext::ChooseModernPixelFormat()
	{
		const int attribs[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_ALPHA_BITS_ARB, 8,
			WGL_SAMPLE_BUFFERS_ARB, 1,
			WGL_SAMPLES_ARB, 4,
			0
		};

		int pixelFormat = 0;
		UINT numFormats = 0;

        if (!wglChoosePixelFormatARB(m_tempHDC ? m_tempHDC : m_hDC, attribs, nullptr, 1, &pixelFormat, &numFormats) || numFormats == 0)
		{
			RNX_LOG_ERROR("wglChoosePixelFormatARB failed");
			return 0;
		}

		return pixelFormat;
	}

	bool Win32GLContext::SetModernPixelFormat(int pixelFormat)
	{
		PIXELFORMATDESCRIPTOR pfd = { 0 };
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);

		int describeResult = DescribePixelFormat(m_hDC, pixelFormat, sizeof(pfd), &pfd);

		if (!describeResult)
		{
			DWORD error = GetLastError();
			RNX_LOG_ERROR("DescribePixelFormat failed. Error code: {0}", error);
			return false;
		}

		if (!SetPixelFormat(m_hDC, pixelFormat, &pfd))
		{
			DWORD error = GetLastError();
			RNX_LOG_ERROR("SetPixelFormat failed. Error code: {0}", error);

			int currentFormat = GetPixelFormat(m_hDC);
			RNX_LOG_INFO("Current pixel format before failure: {0}", currentFormat);

			return false;
		}
	}

	HGLRC Win32GLContext::CreateModernContext()
	{
		int attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, m_majorVersion,
			WGL_CONTEXT_MINOR_VERSION_ARB, m_minorVersion,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			WGL_CONTEXT_FLAGS_ARB, 0,
			0
		};

#ifdef _DEBUG
		attribs[7] = WGL_CONTEXT_DEBUG_BIT_ARB;
#endif

		HGLRC hRC = wglCreateContextAttribsARB(m_hDC, nullptr, attribs);
		if (!hRC)
		{
			if (m_majorVersion == 4 && m_minorVersion == 6)
			{
				RNX_LOG_WARNING("OpenGL 4.6 not supported, trying 4.5");
				attribs[1] = 4; // major
				attribs[3] = 5; // minor
				hRC = wglCreateContextAttribsARB(m_hDC, nullptr, attribs);
			}

			if (!hRC && m_majorVersion == 4 && m_minorVersion == 5)
			{
				RNX_LOG_WARNING("OpenGL 4.5 not supported, trying 4.4");
				attribs[3] = 4;
				hRC = wglCreateContextAttribsARB(m_hDC, nullptr, attribs);
			}

			if (!hRC && m_majorVersion == 4 && m_minorVersion == 4)
			{
				RNX_LOG_WARNING("OpenGL 4.4 not supported, trying 4.3");
				attribs[3] = 3;
				hRC = wglCreateContextAttribsARB(m_hDC, nullptr, attribs);
			}

			if (!hRC)
			{
				RNX_LOG_WARNING("Trying OpenGL 3.3");
				attribs[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
				attribs[1] = 3;
				attribs[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
				attribs[3] = 3;
				attribs[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
				attribs[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
				attribs[6] = WGL_CONTEXT_FLAGS_ARB;
				attribs[7] = 0;
				attribs[8] = 0;
				hRC = wglCreateContextAttribsARB(m_hDC, nullptr, attribs);
			}
		}

		if (!hRC)
		{
			RNX_LOG_ERROR("Failed to create OpenGL context with any version");
		}

		return hRC;
	}

	bool Win32GLContext::CreateTempWindowAndHDC()
	{
		WNDCLASSW wc{ 0 };
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = DefWindowProcW;
		wc.hInstance = GetModuleHandleW(nullptr);
		wc.lpszClassName = L"RynoxDummyGLWindow";

		if (!RegisterClassW(&wc)) {
			return false;
		}

		m_tempHWnd = CreateWindowW(wc.lpszClassName, L"", WS_OVERLAPPEDWINDOW,
			0, 0, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);

		if (!m_tempHWnd) {
			RNX_LOG_ERROR("Failed to create temporary window for GL context");
			return false;
		}

		m_tempHDC = GetDC(m_tempHWnd);
		if (!m_tempHDC) {
			RNX_LOG_ERROR("Failed to get DC for temporary window");
			DestroyWindow(m_tempHWnd);
			m_tempHWnd = nullptr;
			return false;
		}

		return true;
	}

	void Win32GLContext::ReleaseTempContext()
	{
		if (m_tempHRC)
		{ 
			wglMakeCurrent(nullptr, nullptr);
			wglDeleteContext(m_tempHRC);
			m_tempHRC = nullptr;
		}

		if (m_tempHDC && m_tempHWnd) 
		{ 
			ReleaseDC(m_tempHWnd, m_tempHDC);
			DestroyWindow(m_tempHWnd);
			m_tempHDC = nullptr;
			m_tempHWnd = nullptr; 
		}
	}
	void* Win32GLContext::LoadProc(const char* name)
	{
		void* p = (void*)wglGetProcAddress(name);

		if (p && p != (void*)0x1 && p != (void*)0x2 && p != (void*)0x3 && p != (void*)-1)
		{
			return p;
		}

		if (!Win32GLContext::s_opengl32)
			return nullptr;

		return (void*)GetProcAddress(Win32GLContext::s_opengl32, name);
	}
}