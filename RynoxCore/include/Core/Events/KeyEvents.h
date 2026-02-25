#pragma once

#include "IEvent.h"

namespace Rynox::Core 
{
	class KeyEvent : public IEvent 
	{
	public:
		[[nodiscard]] int GetScancode() const { return m_Scancode; }

		RNX_EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard);
	protected:
		KeyEvent(int scancode)
			: m_Scancode(scancode) {}

		int m_Scancode;
	};

	class KeyDownEvent : public KeyEvent 
	{
	public:
		KeyDownEvent(int scancode)
			: KeyEvent(scancode) {}

		RNX_EVENT_CLASS_TYPE(KeyDown);
	};

	class KeyUpEvent : public KeyEvent 
	{
	public:
		KeyUpEvent(int scancode)
			: KeyEvent(scancode) {}
		
		RNX_EVENT_CLASS_TYPE(KeyUp);
	};

	class KeyCharEvent : public KeyEvent 
	{
	public:
		KeyCharEvent(int scancode, int codepoint)
			: KeyEvent(scancode)
			, m_Codepoint(codepoint) {}

		[[nodiscard]] int GetCodepoint() const { return m_Codepoint; }

		RNX_EVENT_CLASS_TYPE(KeyChar);
	private:
		int m_Codepoint;
	};
}
