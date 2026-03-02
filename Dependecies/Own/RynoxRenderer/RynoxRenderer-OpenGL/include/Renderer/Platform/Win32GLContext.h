#pragma once

#include "Renderer/OpenGL/IGraphicsContext.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <gl/GL.h>
#include "Renderer/Vendor/wglext.h"

#include <atomic>

namespace Rynox::Renderer::Platform
{
	class Win32GLContext : public OpenGL::IGraphicsContext
	{
	public:
		Win32GLContext(HWND hwnd);
		~Win32GLContext();

		bool Init() override;
		void SwapBuffers() override;
		bool MakeCurrent() override;
		void* GetOpenGLLoader() override;
	private:
		HGLRC CreateTempContext();
		bool LoadWGLFunctions();
		int ChooseModernPixelFormat();
		bool SetModernPixelFormat(int pixelFormat);
		HGLRC CreateModernContext();
		bool CreateTempWindowAndHDC();
		void ReleaseTempContext();

        static void* LoadProc(const char* name);
    public:
        static HMODULE s_opengl32;
        static std::atomic<int> s_opengl32RefCount;
    private:
		HDC m_hDC{ nullptr };
		HGLRC m_hRC{ nullptr };
		HWND m_hWnd{ nullptr };

		HWND m_tempHWnd{ nullptr };
		HDC m_tempHDC{ nullptr };
		HGLRC m_tempHRC{ nullptr };

		int m_majorVersion{ 4 };
		int m_minorVersion{ 6 };

		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB{ nullptr };
		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB{ nullptr };
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT{ nullptr };
	};
}