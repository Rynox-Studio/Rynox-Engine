#include "App/Application.h"

#include "Platform/Platform.h"
#include "Platform/ModuleService.h"

#include <Common/Assert.h>
#include <chrono>
#include <ranges>

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
			wndDesc.Flags = WindowFlag::Resizable | WindowFlag::Minimizable | WindowFlag::Maximizable | WindowFlag::Visible;
			wndDesc.EventCallback = RNX_BIND_EVENT_FN(RaiseEvent);

			if (!m_Window || !m_Window->Initialize(wndDesc))
			{
				RNX_LOG_ERROR("[Application] Failed to initialize Window.");
				return false;
			}
		}

		if (!m_ModuleService.Initialize())
			return false;

		// TODO: Add logs
		if ((m_Desc.Flags & ApplicationFlagHeadless) == 0)
		{
			if (!m_ModuleService.LoadModule(RYNOX_OPENGL_MODULE_FILENAME, "Renderer"))
				return false;

			auto* module = dynamic_cast<IRendererModule*>(m_ModuleService.GetModule("Renderer"));
			if (!module || !module->Initialize())
				return false;

			RendererDesc rDesc = {};
			rDesc.nWindow = m_Window->GetNativeWindow();

			m_Renderer = module->GetRenderer();
			if (!m_Renderer || !m_Renderer->Initialize(rDesc))
				return false;

			m_Renderer->SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
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

			m_Renderer->BeginFrame({});
			m_Renderer->EndFrame();
		}
	}

	void Application::Stop()
	{
		m_Running = false;
	}

	void Application::RaiseEvent(IEvent& e)
	{
		EventDispatcher d(e);
		d.Dispatch<WindowResizeEvent>(RNX_BIND_EVENT_FN(OnWindowResize));;
		d.Dispatch<WindowCloseEvent>(RNX_BIND_EVENT_FN(OnWindowClose));

		for (auto& layer : m_LayerStack | std::views::reverse)
		{
			if (e.Handled)
				break;
			layer->OnEvent(e);
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Stop();
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		Math::Vec2 size = e.GetSize();
		if (m_Renderer)
		{
			m_Renderer->SetOutputSize(size.x, size.y);
		}

		return false;
	}
}
