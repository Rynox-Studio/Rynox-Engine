#pragma once

#include <string>

#include "Core/LayerStack.h"
#include "Core/IWindow.h"

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

	private:
		bool m_Initialized = false;
		bool m_Running = false;

		LayerStack m_LayerStack;
		std::unique_ptr<IWindow> m_Window;

		static Application* s_Instance;
	};
}
