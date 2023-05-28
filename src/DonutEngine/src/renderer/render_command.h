#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "renderer_api.h"

namespace Donut
{
	class RenderCommand
	{
	public:
		inline static void init()
		{
			renderer_api_->init();
		}
		inline static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			renderer_api_->setViewport(x, y, width, height);
		}
		inline static void setClearColor(const glm::vec4& color)
		{
			renderer_api_->setClearColor(color);
		}

		inline static void clear()
		{
			renderer_api_->clear();
		}

		inline static void drawIndices(const Donut::Ref<VertexArray>& va, uint32_t count = 0)
		{
			renderer_api_->drawIndices(va, count);
		}
	private:
		static RendererAPI* renderer_api_;
	};
}
#endif