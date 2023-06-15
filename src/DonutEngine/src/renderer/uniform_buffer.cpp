#include "pch.h"
#include "uniform_buffer.h"

#include "renderer/renderer.h"
#include "platform/opengl/opengl_uniform_buffer.h"

namespace Donut
{
	Ref<UniformBuffer> UniformBuffer::create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::getRendererApi())
		{
		case RendererAPI::RendererAPIType::None:    DN_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::RendererAPIType::OpenGL:  return createRef<OpenGLUniformBuffer>(size, binding);
		}

		DN_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}