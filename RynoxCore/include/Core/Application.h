#pragma once

#include <string>
#include <memory>

#include <Core/Graphics/IRenderer.h>
#include <Core/LayerStack.h>
#include <Core/IWindow.h>
#include <Core/Services/ModuleService.h>

#include <Core/Events/IEvent.h>
#include <Core/Events/WindowEvents.h>
#include <Core/Events/KeyEvents.h>

namespace Rynox::Core
{
	struct ApplicationDesc
	{
		std::string Name = "Rynox Application";
		GraphicsAPI GraphicsAPI = GraphicsAPI::Default;
	};

	class Application
	{
	public:
		Application();
		~Application();

		static Application& Get();

		bool Initialize(const ApplicationDesc& desc);
		void Run();
		void Stop();

		void RaiseEvent(IEvent& e);

		const ApplicationDesc& GetDesc() const;

		// LayerStack

		void PushLayer(ILayer* layer);
		void PushOverlay(ILayer* overlay);

		void PopLayer(ILayer* layer);
		void PopOverlay(ILayer* overlay);

		// Other

		IWindow* Window();
		IRenderer* Renderer();

	private:
		bool InitServices();
		bool InitSystems();
		bool InitModules();

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnKeyDown(KeyDownEvent& e);

		bool ReloadRenderer();
		bool SwitchRenderer(GraphicsAPI api);

	private:
		ApplicationDesc m_Desc;
		bool m_Initialized = false;
		bool m_Running = false;

		LayerStack m_LayerStack;

		std::unique_ptr<IWindow> m_Window;
		std::unique_ptr<Service::ModuleService> m_ModuleService;
		IRenderer* m_Renderer = nullptr;

		static Application* s_Instance;
	};
}
