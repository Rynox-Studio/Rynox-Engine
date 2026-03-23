#include "App/Application.h"

#include <chrono>
#include <Common/Assert.h>

#include <Rendering/IRendererModule.h>
#include <Rendering/IRenderer.h>

#include <Platform/Platform.h>

namespace Rynox
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		RNX_ASSERT(s_Instance == nullptr);
		s_Instance = this;
	}

	Application::~Application()
	{
		if (s_Instance == this)
		{
			s_Instance = nullptr;
		}
		Platform::DestroyWindow(m_Window);
	}

	Application& Application::Get()
	{
		RNX_ASSERT(s_Instance != nullptr);
		return *s_Instance;
	}

	bool Application::Initialize(const ApplicationDesc& desc)
	{
		if (m_Running) return false;
		m_Initialized = false;

		m_Desc = desc;

		// Window
		m_Window = Platform::CreateWindow();
		{
			WindowDesc wndDesc;
			wndDesc.Title = desc.Name;
			wndDesc.EventCallback = RNX_BIND_EVENT_FN(RaiseEvent);

			if (!m_Window || !m_Window->Initialize(wndDesc))
			{
				RNX_LOG_ERROR("[Application] Failed to initialize Window.");
				return false;
			}
		}

		m_Initialized = true;
		return true;
	}

	void Application::Run()
	{
		if (!m_Initialized || m_Running) return;

		m_Running = true;

		auto last = std::chrono::steady_clock::now();
		while (m_Running)
		{
			auto now = std::chrono::steady_clock::now();
			float dt = std::chrono::duration<float>(now - last).count();
			last = now;

			m_Window->PollEvents();
		}
	}

	void Application::Stop()
	{
		m_Running = false;
	}

	void Application::RaiseEvent(IEvent& e)
	{
		EventDispatcher d(e);
		d.Dispatch<KeyDownEvent>(RNX_BIND_EVENT_FN(OnKeyDown));
		d.Dispatch<WindowResizeEvent>(RNX_BIND_EVENT_FN(OnWindowResize));
		d.Dispatch<WindowCloseEvent>(RNX_BIND_EVENT_FN(OnWindowClose));
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Stop();

		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		int width = e.GetWidth();
		int height = e.GetHeight();

		return false;
	}

	bool Application::OnKeyDown(KeyDownEvent& e)
	{
		int scancode = e.GetScancode();

		return false;
	}
}
