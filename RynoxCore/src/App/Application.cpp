#include "App/Application.h"

#include <chrono>
#include <Common/Assert.h>

#include <Event/WindowEvents.h>

#include <Rendering/IRendererModule.h>
#include <Rendering/IRenderer.h>

#include <Platform/Platform.h>

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

		m_Desc = desc;

		m_Window = std::unique_ptr<IWindow>(Platform::CreateWindow());
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

		if (!InitServices())
		{
			RNX_LOG_ERROR("[Application] Failed to initialize Services.");
			return false;
		}

		if (!InitModules())
		{
			RNX_LOG_ERROR("[Application] Failed to initialize Modules.");
			return false;
		}

		if (!InitSystems())
		{
			RNX_LOG_ERROR("[Application] Failed to initialize Systems.");
			return false;
		}

		// Renderer
		{
			auto size = m_Window->GetSize();

			RendererDesc rendererDesc;
			rendererDesc.nWindow = m_Window->GetNativeHandle();
			rendererDesc.nDisplay = nullptr;
			rendererDesc.viewport = { 0, 0, (uint32_t)size.x, (uint32_t)size.y };
			rendererDesc.outputWidth = (uint32_t)size.x;
			rendererDesc.outputHeight = (uint32_t)size.y;

			if (!m_Renderer->Initialize(rendererDesc))
			{
				RNX_LOG_ERROR("[Application] Failed to initialize Renderer.");
				return false;
			}
		}

		m_Initialized = true;
		return true;
	}

	bool Application::InitServices()
	{
		m_ModuleService = std::make_unique<ModuleService>();
		if (!m_ModuleService->Initialize())
		{
			RNX_LOG_ERROR("[Application] Failed to initialize Module Service.");
			return false;
		}

		return true;
	}

	bool Application::InitSystems()
	{
		m_RenderSystem = std::make_unique<RenderSystem>(m_Renderer);
		if (!m_RenderSystem->Initialize())
		{
			return false;
		}
		return true;
	}

	bool Application::InitModules()
	{
		switch (m_Desc.GraphicsAPI)
		{
			case GraphicsAPI::OpenGL:
			{
				if (!m_ModuleService->LoadModule(RYNOX_OPENGL_MODULE_FILENAME, "Renderer"))
				{
					RNX_LOG_ERROR("[Application] Failed to load Renderer Module. (OpenGL)");
					return false;
				}
			} break;

			case GraphicsAPI::DirectX12:
			{
				if (!m_ModuleService->LoadModule(RYNOX_DIRECTX12_MODULE_FILENAME, "Renderer"))
				{
					RNX_LOG_ERROR("[Application] Failed to load Renderer Module. (DirectX12)");
					return false;
				}
			} break;
		}

		IRendererModule* rendererModule = dynamic_cast<IRendererModule*>(m_ModuleService->GetModule("Renderer"));
		if (!rendererModule)
		{
			RNX_LOG_ERROR("[Application] Failed to cast IModule to IRendererModule.");
			return false;
		}

		if (!rendererModule->Initialize())
		{
			RNX_LOG_ERROR("[Application] Failed to initailize Renderer Module.");
			return false;
		}
		m_Renderer = rendererModule->GetRenderer();

		return true;
	}

	void Application::Run()
	{
		if (!m_Initialized || m_Running) return;

		m_Running = true;

		auto start = std::chrono::steady_clock::now();
		auto last = std::chrono::steady_clock::now();
		while (m_Running)
		{
			auto now = std::chrono::steady_clock::now();
			float time = std::chrono::duration<float>(now - start).count();
			float dt = std::chrono::duration<float>(now - last).count();
			last = now;

			m_Window->PollEvents();

			for (auto& layer : m_LayerStack)
			{
				layer->OnUpdate(dt);
			}

			if (m_RenderSystem->GetRenderer())
			{
				for (auto& layer : m_LayerStack)
				{
					layer->OnRender();
				}
				m_RenderSystem->OnUpdate(dt, time);
			}
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
		d.Dispatch<KeyDownEvent>(RNX_BIND_EVENT_FN(OnKeyDown));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	const ApplicationDesc& Application::GetDesc() const
	{
		return m_Desc;
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

	IWindow* Application::Window()
	{
		return m_Window.get();
	}

	IRenderer* Application::Renderer()
	{
		return m_Renderer;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Stop();
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (m_Renderer)
		{
			m_Renderer->SetOutputSize(e.GetWidth(), e.GetHeight());
			m_Renderer->SetViewport({ 0, 0, (uint32_t)e.GetWidth(), (uint32_t)e.GetHeight() });
		}
		return true;
	}

	bool Application::OnKeyDown(KeyDownEvent& e)
	{
		RNX_LOG_DEBUG("KeyDown: 0x{:X}", e.GetScancode());
		switch (e.GetScancode())
		{
			// F1 - Reload Renderer
			case 0x3B:
			{
				RNX_LOG_INFO("[Application] Reloading Renderer...");
				if (!ReloadRenderer())
				{
					RNX_LOG_INFO("[Application] Failed to reload Renderer.");
					return true;
				}

				RNX_LOG_INFO("[Application] Successfully reloaded.");
			} break;

			// F2 - Swicth Renderer
			case 0x3C:
			{
				RNX_LOG_INFO("[Application] Swicthing Renderer.");

				if (!SwitchRenderer(GraphicsAPI(((int)m_Desc.GraphicsAPI + 1) % (int)GraphicsAPI::Count)))
				{
					RNX_LOG_INFO("[Application] Failed to reload Renderer.");
					return true;
				}

				RNX_LOG_INFO("[Application] Successfully switched.");
			} break;

			default: return false;
		}

		return true;
	}

	bool Application::ReloadRenderer()
	{
		RendererDesc desc{};
		if (m_Renderer)
		{
			desc = m_Renderer->GetDesc();
			m_Renderer = nullptr;
		}
		desc.nWindow = m_Window->GetNativeHandle();

		if (m_ModuleService->GetModule("Renderer"))
		{
			m_ModuleService->UnloadModule("Renderer");
		}

		switch (m_Desc.GraphicsAPI)
		{
			case GraphicsAPI::DirectX12:
			{
				if (!m_ModuleService->LoadModule(RYNOX_DIRECTX12_MODULE_FILENAME, "Renderer"))
				{
					RNX_LOG_ERROR("[Application] Failed to load Renderer Module. (DirectX12)");
					return false;
				}
			} break;

			case GraphicsAPI::OpenGL:
			{
				if (!m_ModuleService->LoadModule(RYNOX_OPENGL_MODULE_FILENAME, "Renderer"))
				{
					RNX_LOG_ERROR("[Application] Failed to load Renderer Module. (OpenGL)");
					return false;
				}
			} break;
		}

		IRendererModule* module = dynamic_cast<IRendererModule*>(m_ModuleService->GetModule("Renderer"));
		if (!module)
		{
			RNX_LOG_ERROR("[Application] Failed to cast IModule to IRendererModule.");
			return false;
		}

		if (!module->Initialize())
		{
			RNX_LOG_ERROR("[Application] Failed to initialized IRendererModule.");
			return false;
		}

		if (!module->GetRenderer()->Initialize(desc))
		{
			RNX_LOG_ERROR("[Application] Failed to initialize Renderer.");
			return false;
		}

		m_Renderer = module->GetRenderer();
		m_RenderSystem->SetRenderer(m_Renderer);
		return true;
	}

	bool Application::SwitchRenderer(GraphicsAPI api)
	{
		m_Desc.GraphicsAPI = api;
		return ReloadRenderer();
	}
}
