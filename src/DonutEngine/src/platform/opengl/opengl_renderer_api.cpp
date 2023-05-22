#include "pch.h"
#include "opengl_renderer_api.h"


#include <glad/glad.h>
namespace Donut
{
	void OpenGLRendererAPI::setClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::drawIndices(const Donut::Ref<VertexArray>& va)
	{
		glDrawElements(GL_TRIANGLES, va->getIndexBuffer()->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
	}
}