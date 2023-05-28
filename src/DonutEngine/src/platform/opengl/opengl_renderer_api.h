#ifndef OPENGL_RENDERER_H
#define OEPNGL_RENDERER_H

#include "renderer/renderer_api.h"

namespace Donut
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void init() override;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;
		virtual void drawIndices(const Donut::Ref<VertexArray>& va, uint32_t count = 0) override;
	private:

	};
}
#endif