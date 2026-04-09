#pragma once

#include <string>
#include <functional>

#include <Event/IEvent.h>
#include <Math/Vec2.h>

namespace Rynox
{
	using WindowFlags = uint32_t;
	namespace WindowFlag
	{
		enum : WindowFlags
		{
			None = 0,

			Resizable = RNX_BIT(0),
			Minimizable = RNX_BIT(1),
			Maximizable = RNX_BIT(2),
			Minimized = RNX_BIT(3),
			Maximized = RNX_BIT(4),
			Visible = RNX_BIT(5),
			Borderless = RNX_BIT(6),
			AlwaysOnTop = RNX_BIT(7),
			CaptureMouse = RNX_BIT(8),
		};
	}

	enum class CursorType : uint32_t
	{
		None = 0, 			// Non-visible cursor
		Arrow,				// Default cursor
		Ibeam,				// For text selecting
		Wait,				// For busy operations
		Crosshair,			// For precise selection
		ResizeVertical,
		ResizeHorizontal,
		ResizeLR,			// Diagonal resize (goes from top left - bottom right)
		ResizeRL,			// Diagonal resize (goes from top right - bottom left)
		SizeAll, 			// SizeAll / Move - Size arrows in all directions
		NotAllowed,			// For unavailable operations
		Help
	};

	struct WindowDesc 
	{
		std::string Title = "Rynox Window";
		Math::Vec2 Position{ 0, 0 };
		Math::Vec2 Size{ 1280, 720 };
		Math::Vec2 MinSize{ 800, 450 };
		Math::Vec2 MaxSize{ 1920, 1080 };
		WindowFlags Flags = WindowFlag::Resizable | WindowFlag::Minimizable | WindowFlag::Maximizable | WindowFlag::Visible;
		CursorType CursorType = CursorType::Arrow;

		std::function<void(IEvent&)> EventCallback = nullptr;
	};

	class IWindow
	{
	public:
		virtual ~IWindow() = default;

		virtual bool Initialize(const WindowDesc& desc) = 0;

		virtual void SetTitle(std::string_view title) = 0;
		virtual void SetPosition(Math::Vec2 position) = 0;
		virtual void SetSize(Math::Vec2 size) = 0;
		virtual void SetMinSize(Math::Vec2 minSize) = 0;
		virtual void SetMaxSize(Math::Vec2 maxSize) = 0;
		virtual void SetEventCallback(std::function<void(IEvent&)> callback) = 0;
		virtual void SetCursorType(CursorType type) = 0;
		
		virtual void SetResizable(bool resizable) = 0;
		virtual void SetMinimizable(bool minimizable) = 0;
		virtual void SetMaximizable(bool maximizable) = 0;
		virtual void SetVisible(bool visible) = 0;
		virtual void SetBorderless(bool borderless) = 0;
		virtual void SetAlwaysOnTop(bool enable) = 0;
		virtual void SetCaptureMouse(bool capture) = 0;

		[[nodiscard]] virtual void* GetNativeWindow() = 0;
		[[nodiscard]] virtual std::string GetTitle() const = 0;
		[[nodiscard]] virtual Math::Vec2 GetPosition() const = 0;
		[[nodiscard]] virtual Math::Vec2 GetSize() const = 0;
		[[nodiscard]] virtual Math::Vec2 GetMinSize() const = 0;
		[[nodiscard]] virtual Math::Vec2 GetMaxSize() const = 0;
		[[nodiscard]] virtual WindowFlags GetFlags() const = 0;
		[[nodiscard]] virtual CursorType GetCursorType() const = 0;

		bool HasFlags(WindowFlags flags) const { return GetFlags() & flags; }

		virtual void PollEvents() = 0;

		virtual void Minimize() = 0;
		virtual void Maximize() = 0;
		virtual void Restore() = 0;

		[[nodiscard]] virtual double GetContentScale() const = 0;
	};
}
