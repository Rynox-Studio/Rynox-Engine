#pragma once

#include <string>
#include <functional>
#include <memory>

#include "Events/IEvent.h"

namespace Rynox::Core 
{
	class IWindow;

	struct WindowDesc 
	{
		std::string Title = "Venom";
		int Position[2] = { 0, 0 };
		int Size[2] = { 1280, 720 };

		bool Show = true;
		bool Focus = true;
		IWindow* Parent = nullptr;
		std::function<void(IEvent&)> EventCallback = nullptr;
	};

	class IWindow 
	{
	public:
		virtual ~IWindow() = default;

		[[nodiscard]] virtual bool Initialize(const WindowDesc& desc) = 0;

		[[nodiscard]] virtual std::string GetTitle() const = 0;
		virtual void GetPosition(int* x, int* y) const = 0;
		virtual void GetSize(int* width, int* height) const = 0;

		virtual void SetTitle(const std::string& title) = 0;
		virtual void SetPosition(int x, int y) = 0;
		virtual void SetSize(int width, int height) = 0;
		virtual void SetEventCallback(std::function<void(IEvent&)> callback) = 0;

		virtual void PollEvents() = 0;
		[[nodiscard]] virtual void* GetNativeHandle() = 0;

		virtual void Show() = 0;
		virtual void Hide() = 0;
		[[nodiscard]] virtual bool IsShown() = 0;

		[[nodiscard]] virtual bool ShouldClose() = 0;

		[[nodiscard]] static std::unique_ptr<IWindow> Create();
	};
}
