#pragma once

#include <string>

#include <Core/Graphics/IRenderer.h>
#include "Core/LayerStack.h"
#include "Core/IWindow.h"
#include "Core/Services/ModuleService.h"

#include "Events/IEvent.h"

namespace Rynox::Core
{
	struct ApplicationDesc
	{
		std::string Name = "Rynox Application";
	};

	class Application
	{
	public:
		Application();
		~Application();

		static Application& Get();

		bool Initialize(const ApplicationDesc& desc);
		bool InitServices();
		bool InitSystems();
		bool InitModules();
		void Run();
		void Stop();

		void RaiseEvent(IEvent& e);

		// LayerStack

		void PushLayer(ILayer* layer);
		void PushOverlay(ILayer* overlay);

		void PopLayer(ILayer* layer);
		void PopOverlay(ILayer* overlay);

		// Window

		IWindow& GetWindow();
	private:
		bool OnWindowClose(IEvent& e);
		bool OnWindowResize(IEvent& e);
	private:
		bool m_Initialized = false;
		bool m_Running = false;

		LayerStack m_LayerStack;

		std::unique_ptr<IWindow> m_Window;
		std::unique_ptr<Service::ModuleService> m_ModuleService;
		IRenderer* m_Renderer;

		static Application* s_Instance;
	};
}
