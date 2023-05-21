#ifndef RENDERER_H
#define RENDERER_H

#include "renderer/renderer_api.h"
#include "renderer/render_command.h"

namespace Donut
{
	class Renderer
	{
	public:
		static void beginScene();
		static void endScene();
		static void submit(const std::shared_ptr<VertexArray>& va);
		inline static RendererAPI::RendererAPIType getRendererApi() { return s_type_; }
	private:
		static RendererAPI::RendererAPIType s_type_;
	};
}


#endif