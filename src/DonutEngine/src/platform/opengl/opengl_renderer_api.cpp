#include "pch.h"
#include "opengl_renderer_api.h"


#include <glad/glad.h>
namespace Donut
{
	void OpenGLRendererAPI::init()
	{
		DN_PROFILE_FUNCTION();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
	}

	void OpenGLRendererAPI::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::setClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::drawIndices(const Donut::Ref<VertexArray>& va, uint32_t count)
	{
		va->bind();
		uint32_t indices_count = count ? count : va->getIndexBuffer()->getIndicesCount();
		glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, nullptr);
		//glBindTexture(GL_TEXTURE_2D, 0);
	}
	void OpenGLRendererAPI::drawLines(const Donut::Ref<VertexArray>& va, uint32_t vertex_count)
	{
		va->bind();
		glDrawArrays(GL_LINES, 0, vertex_count);
	}

	void OpenGLRendererAPI::setLineWidth(float width)
	{
		glLineWidth(width);
	}
}