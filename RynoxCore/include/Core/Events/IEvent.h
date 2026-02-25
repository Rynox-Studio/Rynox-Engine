#pragma once

#include <string>
#include <functional>

#include <Common/Macros.h>

namespace Rynox::Core
{
	enum class EventType 
	{
		None = 0,
		WindowMove, WindowResize, WindowClose, WindowFocus,
		KeyDown, KeyUp, KeyChar,
		MouseMove, MouseScroll, MouseButtonDown, MouseButtonUp
	};

	enum EventCategory 
	{
		None = 0,
		EventCategoryWindow = RNX_BIT(0),
		EventCategoryInput = RNX_BIT(1),
		EventCategoryKeyboard = RNX_BIT(2),
		EventCategoryMouse = RNX_BIT(3),
		EventCategoryMouseButton = RNX_BIT(4),
	};

	class IEvent 
	{
	public:
		bool Handled = false;

		virtual ~IEvent() = default;
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		bool IsInCategory(EventCategory category) const 
		{
			return GetCategoryFlags() & category;
		}
	};

#define RNX_EVENT_CLASS_TYPE(type) \
	static EventType GetStaticType() { return EventType::type; } \
	virtual EventType GetEventType() const override { return GetStaticType(); } \
	virtual const char* GetName() const override { return #type; }

#define RNX_EVENT_CLASS_CATEGORY(category) \
	virtual int GetCategoryFlags() const override { return category; }

	class EventDispatcher
	{
	public:
		template<typename T>
		using EventFn = std::function<bool(T&)>;

		EventDispatcher(IEvent& e) : m_Event(e) {}

		template<typename T>
		inline bool Dispatch(EventFn<T> func) {
			if (m_Event.GetEventType() == T::GetStaticType() && !m_Event.Handled) {
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}

	private:
		IEvent& m_Event;
	};

#define RNX_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
}