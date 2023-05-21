#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "renderer_api.h"

namespace Donut
{
	class RenderCommand
	{
	public:
		inline static void setClearColor(const glm::vec4& color)
		{
			renderer_api_->setClearColor(color);
		}

		inline static void clear()
		{
			renderer_api_->clear();
		}

		inline static void drawIndices(const std::shared_ptr<VertexArray>& va)
		{
			renderer_api_->drawIndices(va);
		}
	private:
		static RendererAPI* renderer_api_;
	};
}
#endif