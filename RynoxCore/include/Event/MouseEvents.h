#pragma once

#include "IEvent.h"

namespace Rynox::Core 
{
	class MouseEvent : public IEvent 
	{
	public:
		[[nodiscard]] int GetX() const { return m_X; }
		[[nodiscard]] int GetY() const { return m_Y; }

	protected:
		MouseEvent(int x, int y)
			: m_X(x)
			, m_Y(y) {}

		int m_X, m_Y;
	};

	class MouseMoveEvent : public MouseEvent
	{
	public:
		MouseMoveEvent(int x, int y, int dx, int dy)
			: MouseEvent(x, y)
			, m_DeltaX(dx)
			, m_DeltaY(dy) {}

		[[nodiscard]]int GetDeltaX() const { return m_DeltaX; }
		[[nodiscard]]int GetDeltaY() const { return m_DeltaY; }

		RNX_EVENT_CLASS_TYPE(MouseMove);
		RNX_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse);
	private:
		int m_DeltaX, m_DeltaY;
	};

	class MouseScrollEvent : public MouseEvent
	{
	public:
		MouseScrollEvent(int x, int y, float scroll_x, float scroll_y)
			: MouseEvent(x, y)
			, m_ScrollX(scroll_x)
			, m_ScrollY(scroll_y) {}

		[[nodiscard]] float GetScrollX() const { return m_ScrollX; }
		[[nodiscard]] float GetScrollY() const { return m_ScrollX; }

		RNX_EVENT_CLASS_TYPE(MouseScroll);
		RNX_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse);
	private:
		float m_ScrollX, m_ScrollY;
	};

	class MouseButtonEvent : public MouseEvent
	{
	public:
		[[nodiscard]] int GetButton() const { return m_Button; }

	protected:
		MouseButtonEvent(int x, int y, int button)
			: MouseEvent(x, y)
			, m_Button(button) {}

		int m_Button;
	};

	class MouseButtonDownEvent : public MouseButtonEvent
	{
	public:
		MouseButtonDownEvent(int x, int y, int button)
			: MouseButtonEvent(x, y, button) {}

		RNX_EVENT_CLASS_TYPE(MouseButtonDown);
		RNX_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouseButton);
	};

	class MouseButtonUpEvent : public MouseButtonEvent
	{
	public:
		MouseButtonUpEvent(int x, int y, int button)
			: MouseButtonEvent(x, y, button) {}

		RNX_EVENT_CLASS_TYPE(MouseButtonUp);
		RNX_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouseButton);
	};
}
