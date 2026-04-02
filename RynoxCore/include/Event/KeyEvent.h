#pragma once

#include <Input/KeyCode.h>
#include "IEvent.h"

namespace Rynox
{
	class KeyEvent : public IEvent 
	{
	public:
		[[nodiscard]] KeyCode GetScancode() const { return m_Key; }

		RNX_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard);
	protected:
		KeyEvent(KeyCode key)
			: m_Key(key) {}

		KeyCode m_Key;
	};

	class KeyDownEvent : public KeyEvent 
	{
	public:
		KeyDownEvent(KeyCode key)
			: KeyEvent(key) {}

		RNX_EVENT_CLASS_TYPE(KeyDown);
	};

	class KeyUpEvent : public KeyEvent 
	{
	public:
		KeyUpEvent(KeyCode key)
			: KeyEvent(key) {
		}

		RNX_EVENT_CLASS_TYPE(KeyUp);
	};

	class KeyCharEvent : public KeyEvent
	{
	public:
		KeyCharEvent(KeyCode key, int codepoint)
			: KeyEvent(key)
			, m_Codepoint(codepoint) {}

		[[nodiscard]] int GetCodepoint() const { return m_Codepoint; }

		RNX_EVENT_CLASS_TYPE(KeyChar);
	private:
		int m_Codepoint;
	};
}
