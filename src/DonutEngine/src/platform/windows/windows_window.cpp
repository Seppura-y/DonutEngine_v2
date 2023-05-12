#include "pch.h"
#include "windows_window.h"

#include "events/application_event.h"
#include "events/mouse_event.h"
#include "events/key_event.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>


namespace Donut {

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		DN_ENGINE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		shutdown();
	}

	void WindowsWindow::init(const WindowProps& props)
	{
		win_data_.title = props.title;
		win_data_.width = props.width;
		win_data_.height = props.height;

		DN_ENGINE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);
#ifdef DN_USE_GLFW
		if (!s_GLFWInitialized)
		{
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			DN_CORE_ASSERT(success, "Could not intialize GLFW!");

			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		glfw_window_ = glfwCreateWindow((int)props.width, (int)props.height, win_data_.title.c_str(), nullptr, (GLFWwindow*)props.window);

		//graphics_ctx_ = new OpenGLContext(glfw_window_);
		//graphics_ctx_->init();

		glfwSetWindowUserPointer(glfw_window_, &win_data_);
		setVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(glfw_window_, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);
			data.event_callback(event);
		});

		glfwSetWindowCloseCallback(glfw_window_, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.event_callback(event);
		});

		glfwSetKeyCallback(glfw_window_, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.event_callback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.event_callback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.event_callback(event);
					break;
				}
			}
		});

		glfwSetMouseButtonCallback(glfw_window_, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.event_callback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.event_callback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(glfw_window_, [](GLFWwindow* window, double x_offset, double y_offset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)x_offset, (float)y_offset);
			data.event_callback(event);
		});

		glfwSetCursorPosCallback(glfw_window_, [](GLFWwindow* window, double x_pos, double y_pos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)x_pos, (float)y_pos);
			data.event_callback(event);
		});
#endif
	}

	void WindowsWindow::shutdown()
	{
#ifdef DN_USE_GLFW
		glfwDestroyWindow(glfw_window_);
#endif
	}

	void WindowsWindow::onUpdate()
	{
#ifdef DN_USE_GLFW
		glfwPollEvents();
		//graphics_ctx_->swapBuffers();
#endif
	}

	void WindowsWindow::setVSync(bool enabled)
	{
#ifdef DN_USE_GLFW
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		win_data_.is_vsync = enabled;
#endif
	}

	bool WindowsWindow::isVSync() const
	{
		return win_data_.is_vsync;
	}

}