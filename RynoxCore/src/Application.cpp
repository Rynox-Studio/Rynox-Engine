#include "Core/Application.h"

#include <chrono>
#include <Common/Assert.h>

#include "Core/Events/WindowEvents.h"

#include "Core/Interfaces/IRendererModule.h"

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
			wndDesc.EventCallback = RNX_BIND_EVENT_FN(RaiseEvent);

			if (!m_Window || !m_Window->Initialize(wndDesc))
			{
				return false;
			}
		}

		if (!InitServices())
			return false;

		if (!InitModules())
			return false;

		if (!InitSystems())
			return false;

		{
			RendererDesc rendererDesc;
			rendererDesc.nWindow = m_Window->GetNativeHandle();
			rendererDesc.nDisplay = nullptr;

			int width, height;
			m_Window->GetSize(&width, &height);
			rendererDesc.viewport = { 0, 0, (uint32_t)width, (uint32_t)height };
			rendererDesc.outputWidth = (uint32_t)width;
			rendererDesc.outputHeight = (uint32_t)height;

			m_Renderer->Initialize(rendererDesc);
		}

		m_Initialized = true;
		return true;
	}

	bool Application::InitServices()
	{
		m_ModuleService = std::make_unique<Service::ModuleService>();
		m_ModuleService->Initialize();
		return true;
	}

	bool Application::InitSystems()
	{
		return true;
	}

	bool Application::InitModules()
	{
		m_ModuleService->LoadModule(RYNOX_OPENGL_MODULE_FILENAME, "RendererOpenGL");
		IRendererModule* rendererModule = dynamic_cast<IRendererModule*>(m_ModuleService->GetModule("RendererOpenGL"));
		rendererModule->Initialize();
		m_Renderer = rendererModule->GetRenderer();
		return true;
	}

	void Application::Run()
	{
		if (!m_Initialized || m_Running) return;

		m_Running = true;

		auto startTime = std::chrono::steady_clock::now();
		auto last = std::chrono::steady_clock::now();
		while (m_Running)
		{
			auto now = std::chrono::steady_clock::now();
			float dt = std::chrono::duration<float>(now - last).count();
			last = now;
			
			float totalTime = std::chrono::duration<float>(now - startTime).count();

			m_Window->PollEvents();

			for (auto& layer : m_LayerStack)
			{
				layer->OnUpdate(dt);
			}

			m_Renderer->BeginFrame();
			for (auto& layer : m_LayerStack)
			{
				layer->OnRender();
			}
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
		d.Dispatch<WindowCloseEvent>(RNX_BIND_EVENT_FN(OnWindowClose));
		d.Dispatch<WindowResizeEvent>(RNX_BIND_EVENT_FN(OnWindowResize));

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

	bool Application::OnWindowClose(IEvent& e)
	{
		Stop();
		return true;
	}
	bool Application::OnWindowResize(IEvent& e)
	{
		WindowResizeEvent& event = (WindowResizeEvent&)e;
		m_Renderer->SetViewport({ 0, 0, (uint32_t)event.GetWidth(), (uint32_t)event.GetHeight() });
		return true;
	}
}
