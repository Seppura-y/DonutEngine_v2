#ifndef OPENGL_CONTEXT_H
#define OPENGL_CONTEXT_H

#include "renderer/graphics_context.h"

struct GLFWwindow;

namespace Donut
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* win_handle);
		virtual void init() override;
		virtual void swapBuffers() override;

	private:
		GLFWwindow* win_handle_;
	};
}
#endif