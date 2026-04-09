#pragma once

#include <Input/KeyCode.h>
#include "IEvent.h"

namespace Rynox
{
	class IKeyEvent : public IEvent 
	{
	public:
		[[nodiscard]] KeyCode GetKey() const { return m_Key; }

		RNX_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard);
	protected:
		IKeyEvent(KeyCode key)
			: m_Key(key) {}

		KeyCode m_Key;
	};

	class KeyDownEvent : public IKeyEvent 
	{
	public:
		KeyDownEvent(KeyCode key)
			: IKeyEvent(key) {}

		RNX_EVENT_CLASS_TYPE(KeyDown);
	};

	class KeyUpEvent : public IKeyEvent 
	{
	public:
		KeyUpEvent(KeyCode key)
			: IKeyEvent(key) {}

		RNX_EVENT_CLASS_TYPE(KeyUp);
	};

	class KeyCharEvent : public IKeyEvent
	{
	public:
		KeyCharEvent(KeyCode key, char32_t codepoint)
			: IKeyEvent(key)
			, m_Codepoint(codepoint) {}

		[[nodiscard]] char32_t GetCodepoint() const { return m_Codepoint; }

		RNX_EVENT_CLASS_TYPE(KeyChar);
	private:
		char32_t m_Codepoint;
	};
}
