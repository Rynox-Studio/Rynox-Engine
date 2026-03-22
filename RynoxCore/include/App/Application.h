#pragma once

#include <string>
#include <memory>

#include <Rendering/IRenderer.h>
#include <App/LayerStack.h>
#include <Platform/IWindow.h>
#include <Platform/ModuleService.h>
#include <Rendering/RenderSystem.h>

#include <Event/IEvent.h>
#include <Event/WindowEvents.h>
#include <Event/KeyEvents.h>

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
		std::unique_ptr<ModuleService> m_ModuleService;
		std::unique_ptr<RenderSystem> m_RenderSystem;
		IRenderer* m_Renderer = nullptr;

		static Application* s_Instance;
	};
}
