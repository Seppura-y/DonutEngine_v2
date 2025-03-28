#ifndef WINDOWS_INPUT_H
#define WINDOWS_INPUT_H

#include "core/input.h"

namespace Donut
{
	class WindowsInput : public Input
	{
	public:
		WindowsInput();
	protected:
		virtual bool isKeyPressedImpl(int keycode) override;
		virtual bool isMouseButtonPressedImpl(int button) override;
		virtual float getMouseXImpl() override;
		virtual float getMouseYImpl() override;
		virtual std::pair<float, float> getMousePositionImpl() override;
	};
}

#endif