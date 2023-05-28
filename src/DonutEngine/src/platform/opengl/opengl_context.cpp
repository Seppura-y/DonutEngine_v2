#include "pch.h"
#include "opengl_context.h"

#include <glfw/glfw3.h>
#include <glad/glad.h>

#include <iostream>
#include <sstream>

namespace Donut
{
	OpenGLContext::OpenGLContext(GLFWwindow* win_handle)
		:win_handle_(win_handle)
	{
		DN_CORE_ASSERT(win_handle_, "window handle is null");
	}

	void OpenGLContext::init()
	{
		DN_PROFILE_FUNCTION();

		glfwMakeContextCurrent(win_handle_);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		DN_CORE_ASSERT(status, "failed to initialize glad");

		DN_CORE_INFO("OpenGL Info : ");

		std::stringstream ss;
		ss << glGetString(GL_VENDOR);
		DN_CORE_INFO("Vendor	: {0}", ss.str());

		ss.clear();
		ss << glGetString(GL_RENDERER);
		DN_CORE_INFO("Renderer	: {0}", ss.str());

		ss.clear();
		ss << glGetString(GL_VERSION);
		DN_CORE_INFO("Version	: {0}", ss.str());
	}

	void OpenGLContext::swapBuffers()
	{
		DN_PROFILE_FUNCTION();

		glfwSwapBuffers(win_handle_);
	}
}