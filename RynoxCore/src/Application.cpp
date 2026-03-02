#include "Core/Application.h"

#include <chrono>
#include <Common/Assert.h>

#include "Core/Events/WindowEvents.h"
#include "Core/Modules/OpenGLModule.h"

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

		m_Renderer->Initialize(m_Window->GetNativeHandle(), nullptr);

		m_Initialized = true;
		return true;
	}

	bool Application::InitServices()
	{
		m_ModuleService = std::make_unique<Service::ModuleService>("modules");
		m_ModuleService->Initialize();
		return true;
	}

	bool Application::InitModules()
	{
		Service::ModuleService::ErrorCode code = m_ModuleService->LoadModule(ModuleType::OpenGLRenderer);
		if (code != Service::ModuleService::ErrorCode::None)
		{
			RNX_LOG_ERROR("[Application] Failed load module: OpenGLRenderer");
			return false;
		}

		Module::OpenGLModule* openGLModule = static_cast<Module::OpenGLModule*>(m_ModuleService->GetModule(ModuleType::OpenGLRenderer));
		m_Renderer = openGLModule->GetRenderer();
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

			for (auto& layer : m_LayerStack)
			{
				layer->OnUpdate(dt);
			}

			m_Renderer->BeginFrame();
			for (auto& layer : m_LayerStack)
			{
				layer->OnRender();
			}
			m_Renderer->RenderFrame(Graphics::FrameContext());
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

	const IRenderer* Application::GetRenderer()
	{
		return m_Renderer;
	}

	bool Application::OnWindowClose(IEvent& e)
	{
		Stop();
		return true;
	}
}
