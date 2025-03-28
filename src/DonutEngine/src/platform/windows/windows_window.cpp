#include "pch.h"
#include "windows_window.h"

#include "events/application_event.h"
#include "events/mouse_event.h"
#include "events/key_event.h"

#include "platform/opengl/opengl_context.h"

#include <glfw/glfw3.h>


namespace Donut {

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		DN_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		DN_PROFILE_FUNCTION();

		init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		DN_PROFILE_FUNCTION();

		shutdown();
	}

	void WindowsWindow::init(const WindowProps& props)
	{
		DN_PROFILE_FUNCTION();

		win_data_.title = props.title;
		win_data_.width = props.width;
		win_data_.height = props.height;

		DN_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);
		if (!s_GLFWInitialized)
		{
			DN_PROFILE_SCOPE("glfwInit");
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			DN_CORE_ASSERT(success, "Could not intialize GLFW!");

			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		{
			DN_PROFILE_SCOPE("glfwCreateWindow");
			glfw_window_ = glfwCreateWindow((int)props.width, (int)props.height, win_data_.title.c_str(), nullptr, nullptr);
		}
		
		graphics_ctx_ = new OpenGLContext(glfw_window_);
		graphics_ctx_->init();

		//glfwMakeContextCurrent(glfw_window_);
		//int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

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
					KeyPressedEvent event(key, false);
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
					KeyPressedEvent event(key, true);
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

		glfwSetCharCallback(glfw_window_, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent ev(keycode);
			data.event_callback(ev);
		});

		glfwSetDropCallback(glfw_window_, [](GLFWwindow* window, int path_count, const char* paths[])
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			std::vector<std::filesystem::path> filepaths(path_count);
			for (int i = 0; i < path_count; i++)
			{
				filepaths[i] = paths[i];
			}

			WindowDropEvent event(std::move(filepaths));
			data.event_callback(event);
		});
	}

	void WindowsWindow::shutdown()
	{
		DN_PROFILE_FUNCTION();

		glfwDestroyWindow(glfw_window_);
	}

	void WindowsWindow::onUpdate()
	{
		DN_PROFILE_FUNCTION();

		glfwPollEvents();
		graphics_ctx_->swapBuffers();
	}

	void WindowsWindow::setVSync(bool enabled)
	{
		DN_PROFILE_FUNCTION();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		win_data_.is_vsync = enabled;
	}

	bool WindowsWindow::isVSync() const
	{
		return win_data_.is_vsync;
	}

}