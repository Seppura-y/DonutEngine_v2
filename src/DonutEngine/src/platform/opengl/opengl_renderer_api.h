#ifndef OPENGL_RENDERER_H
#define OEPNGL_RENDERER_H

#include "renderer/renderer_api.h"

namespace Donut
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;
		virtual void drawIndices(const Donut::Ref<VertexArray>& va) override;
	private:

	};
}
#endif