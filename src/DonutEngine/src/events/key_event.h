#pragma once

#include "events/event.h"

namespace Donut {

	class DONUT_API KeyEvent : public Event
	{
	public:
		inline int getKeyCode() const { return key_code_; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(int keycode)
			: key_code_(keycode) {}

		int key_code_;
	};

	class DONUT_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount)
			: KeyEvent(keycode), repeat_count_(repeatCount) {}

		inline int getRepeatCount() const { return repeat_count_; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << key_code_ << " (" << repeat_count_ << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int repeat_count_;
	};

	class DONUT_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << key_code_;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};


	class DONUT_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << key_code_;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}