#pragma once

#include <Input/MouseButton.h>
#include "IEvent.h"

namespace Rynox
{
	class IMouseEvent : public IEvent 
	{
	public:
		[[nodiscard]] Math::Vec2 GetPosition() const { return m_Position; }

	protected:
		IMouseEvent(Math::Vec2 position)
			: m_Position(position) {}

		Math::Vec2 m_Position;
	};

	class MouseMoveEvent : public IMouseEvent
	{
	public:
		MouseMoveEvent(Math::Vec2 position, Math::Vec2 delta)
			: IMouseEvent(position)
			, m_Delta(delta) {}

		[[nodiscard]] Math::Vec2 GetDelta() const { return m_Delta; }

		RNX_EVENT_CLASS_TYPE(MouseMove);
		RNX_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse);
	private:
		Math::Vec2 m_Delta;
	};

	class MouseScrollEvent : public IMouseEvent
	{
	public:
		MouseScrollEvent(Math::Vec2 position, Math::Vec2 scroll)
			: IMouseEvent(position)
			, m_Scroll(scroll) {}

		[[nodiscard]] Math::Vec2 GetScroll() const { return m_Scroll; }

		RNX_EVENT_CLASS_TYPE(MouseScroll);
		RNX_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouse);
	private:
		Math::Vec2 m_Scroll;
	};

	class IMouseButtonEvent : public IMouseEvent
	{
	public:
		[[nodiscard]] MouseButton GetButton() const { return m_Button; }

	protected:
		IMouseButtonEvent(Math::Vec2 position, MouseButton button)
			: IMouseEvent(position)
			, m_Button(button) {}

		MouseButton m_Button;
	};

	class MouseButtonDownEvent : public IMouseButtonEvent
	{
	public:
		MouseButtonDownEvent(Math::Vec2 position, MouseButton button)
			: IMouseButtonEvent(position, button) {}

		RNX_EVENT_CLASS_TYPE(MouseButtonDown);
		RNX_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouseButton);
	};

	class MouseButtonUpEvent : public IMouseButtonEvent
	{
	public:
		MouseButtonUpEvent(Math::Vec2 position, MouseButton button)
			: IMouseButtonEvent(position, button) {}

		RNX_EVENT_CLASS_TYPE(MouseButtonUp);
		RNX_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryMouseButton);
	};
}
