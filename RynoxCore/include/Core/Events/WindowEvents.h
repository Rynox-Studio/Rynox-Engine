#pragma once

#include "IEvent.h"

namespace Rynox::Core
{
	class WindowEvent : public IEvent
	{
	public:

	protected:
		WindowEvent() = default;
	};

	class WindowMoveEvent : public WindowEvent
	{
	public:
		WindowMoveEvent(int x, int y)
			: m_X(x)
			, m_Y(y) {}

		[[nodiscard]] int GetX() const { return m_X; }
		[[nodiscard]] int GetY() const { return m_Y; }

		RNX_EVENT_CLASS_TYPE(WindowMove);
		RNX_EVENT_CLASS_CATEGORY(EventCategoryWindow);
	private:
		int m_X, m_Y;
	};

	class WindowResizeEvent : public WindowEvent
	{
	public:
		WindowResizeEvent(int width, int height)
			: m_Width(width)
			, m_Height(height) {}

		[[nodiscard]] int GetWidth() const { return m_Width; }
		[[nodiscard]] int GetHeight() const { return m_Height; }

		RNX_EVENT_CLASS_TYPE(WindowResize);
		RNX_EVENT_CLASS_CATEGORY(EventCategoryWindow);
	private:
		int m_Width, m_Height;
	};

	class WindowCloseEvent : public WindowEvent
	{
	public:
		WindowCloseEvent() = default;

		RNX_EVENT_CLASS_TYPE(WindowClose);
		RNX_EVENT_CLASS_CATEGORY(EventCategoryWindow);
	};

	class WindowFocusEvent : public WindowEvent
	{
	public:
		WindowFocusEvent(bool focused) 
			: m_Focused(focused) {}

		[[nodiscard]] bool IsFocused() const { return m_Focused; }

		RNX_EVENT_CLASS_TYPE(WindowFocus);
		RNX_EVENT_CLASS_CATEGORY(EventCategoryWindow);
	private:
		bool m_Focused;
	};
}
