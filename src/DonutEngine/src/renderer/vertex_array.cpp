#include "pch.h"

#include "renderer/renderer_api.h"
#include "renderer/vertex_array.h"

#include "platform/opengl/opengl_vertex_array.h"

namespace Donut
{
	Ref<VertexArray> VertexArray::create()
	{
		switch (RendererAPI::getCurrentAPIType())
		{
			case RendererAPI::RendererAPIType::None: DN_CORE_ASSERT(false, "RendererAPI::RendererAPIType::None:"); return nullptr;
			case RendererAPI::RendererAPIType::OpenGL: return std::make_shared<OpenGLVertexArray>();
		}
		DN_CORE_ASSERT(false, "unknown renderer api");

		return nullptr;
	}
}