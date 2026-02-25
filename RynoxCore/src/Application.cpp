#include "Core/Application.h"

#include <chrono>

#include <Common/Assert.h>

namespace Rynox::Core
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

		m_Window = IWindow::Create();
		{
			WindowDesc wndDesc;
			wndDesc.Title = desc.Name;

			if (!m_Window->Initialize(wndDesc))
			{
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
		}
	}

	void Application::Stop()
	{
		m_Running = false;
	}

	void Application::RaiseEvent(IEvent& e)
	{
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::PushLayer(ILayer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(ILayer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
	}

	void Application::PopLayer(ILayer* layer)
	{
		m_LayerStack.PopLayer(layer);
	}

	void Application::PopOverlay(ILayer* overlay)
	{
		m_LayerStack.PopOverlay(overlay);
	}

	IWindow& Application::GetWindow()
	{
		return *m_Window.get();
	}
}
