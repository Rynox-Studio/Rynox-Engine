#pragma once

#include <string>
#include <functional>

#include <Core/Events/IEvent.h>
#include <Math/Vec2.h>

namespace Rynox::Core 
{
	class IWindow;

	struct WindowDesc 
	{
		std::string Title = "Rynox Window";
		Math::Vec2 Position{ 0, 0 };
		Math::Vec2 Size{ 1280, 720 };

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
		virtual Math::Vec2 GetPosition() const = 0;
		virtual Math::Vec2 GetSize() const = 0;

		virtual void SetTitle(std::string_view title) = 0;
		virtual void SetPosition(Math::Vec2 position) = 0;
		virtual void SetSize(Math::Vec2 size) = 0;
		virtual void SetEventCallback(std::function<void(IEvent&)> callback) = 0;

		virtual void PollEvents() = 0;
		[[nodiscard]] virtual void* GetNativeHandle() = 0;

		virtual void Show() = 0;
		virtual void Hide() = 0;
		[[nodiscard]] virtual bool IsShown() = 0;

		[[nodiscard]] virtual bool ShouldClose() = 0;
	};
}
