#pragma once

#include <string>

#include <App/LayerStack.h>

#include <Platform/IWindow.h>
#include <Platform/ModuleService.h>

#include <Rendering/IRenderer.h>

#include <Event/IEvent.h>
#include <Event/WindowEvent.h>
#include <Event/KeyEvent.h>

namespace Rynox
{
	enum ApplicationFlags : uint32_t
	{
		ApplicationFlagNone = 0,
		ApplicationFlagHeadless = RNX_BIT(0)
	};

	struct ApplicationDesc
	{
		std::string Name = "Rynox Application";
		ApplicationFlags Flags = ApplicationFlagNone;
		int ArgCount = 0;
		char** ArgVector = nullptr;
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

		const ApplicationDesc& GetDesc() const { return m_Desc; }

		void PushLayer(std::unique_ptr<ILayer> layer) { m_LayerStack.PushLayer(std::move(layer)); }
		void RemoveLayer(ILayer* layer) { m_LayerStack.RemoveLayer(layer); }

		IWindow* Window() { return m_Window; }
		IRenderer* Renderer() { return m_Renderer; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		ApplicationDesc m_Desc;
		bool m_Initialized = false;
		bool m_Running = false;

		LayerStack m_LayerStack;
		ModuleService m_ModuleService;
		IWindow* m_Window = nullptr;
		IRenderer* m_Renderer = nullptr;

		static Application* s_Instance;
	};
}
