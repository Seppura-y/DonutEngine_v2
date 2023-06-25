#include "pch.h"
#include "core/input.h"
#include "platform/windows/windows_input.h"

#include "core/application.h"
#include <glfw/glfw3.h>

namespace Donut
{
	Input* Input::s_instance_ = new WindowsInput();

	WindowsInput::WindowsInput()
	{
		//Input::s_instance_ = this;
	}

	bool WindowsInput::isKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS;
	}
	bool WindowsInput::isMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	float WindowsInput::getMouseXImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
		double x_pos, y_pos;
		glfwGetCursorPos(window, &x_pos, &y_pos);

		return (float)x_pos;

		// cpp 17 structured bindings 结构化绑定
		//auto [x, y] = getMousePositionImpl();
		//return y;
	}

	float WindowsInput::getMouseYImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
		double x_pos, y_pos;
		glfwGetCursorPos(window, &x_pos, &y_pos);

		return (float)y_pos;

		// cpp 17 structured bindings 结构化绑定
		//auto [x, y] = getMousePositionImpl();
		//return y;
	}
	std::pair<float, float> WindowsInput::getMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
		double x_pos, y_pos;
		glfwGetCursorPos(window, &x_pos, &y_pos);
		return { (float)x_pos, (float)y_pos };
	}
}