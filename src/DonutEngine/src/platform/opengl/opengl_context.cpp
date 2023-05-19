#include "pch.h"
#include "opengl_context.h"

#include <glfw/glfw3.h>
#include <glad/glad.h>
namespace Donut
{
	OpenGLContext::OpenGLContext(GLFWwindow* win_handle)
		:win_handle_(win_handle)
	{
		DN_CORE_ASSERT(win_handle_, "window handle is null");
	}

	void OpenGLContext::init()
	{
		glfwMakeContextCurrent(win_handle_);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		DN_CORE_ASSERT(status, "failed to initialize glad");
	}

	void OpenGLContext::swapBuffers()
	{
		glfwSwapBuffers(win_handle_);
	}
}