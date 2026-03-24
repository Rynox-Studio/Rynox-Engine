#pragma once

#include "IEvent.h"
#include <Math/Vec2.h>

namespace Rynox
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
		WindowMoveEvent(Math::Vec2 position)
			: m_Position(position) {}

		[[nodiscard]] Math::Vec2 GetPosition() const { return m_Position; }

		RNX_EVENT_CLASS_TYPE(WindowMove);
		RNX_EVENT_CLASS_CATEGORY(EventCategoryWindow);
	private:
		Math::Vec2 m_Position;
	};

	class WindowResizeEvent : public WindowEvent
	{
	public:
		WindowResizeEvent(Math::Vec2 size)
			: m_Size(size) {}

		[[nodiscard]] Math::Vec2 GetSize() const { return m_Size; }

		RNX_EVENT_CLASS_TYPE(WindowResize);
		RNX_EVENT_CLASS_CATEGORY(EventCategoryWindow);
	private:
		Math::Vec2 m_Size;
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
