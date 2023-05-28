#include "pch.h"
#include "buffers.h"

#include "renderer/renderer_api.h"
#include "platform/opengl/opengl_buffers.h"

namespace Donut
{
	VertexBuffer* VertexBuffer::create(uint32_t size)
	{
		switch (RendererAPI::getCurrentAPIType())
		{
		case RendererAPI::RendererAPIType::None: DN_CORE_ASSERT(false, "RendererAPI::RendererAPIType::None:"); return nullptr;
		case RendererAPI::RendererAPIType::OpenGL: return new OpenGLVertexBuffer(size);
		}
		DN_CORE_ASSERT(false, "unknown renderer api");

		return nullptr;
	}

	VertexBuffer* VertexBuffer::create(float* vertices, uint32_t size)
	{
		switch (RendererAPI::getCurrentAPIType())
		{
			case RendererAPI::RendererAPIType::None: DN_CORE_ASSERT(false, "RendererAPI::RendererAPIType::None:"); return nullptr;
			case RendererAPI::RendererAPIType::OpenGL: return new OpenGLVertexBuffer(vertices, size);
		}
		DN_CORE_ASSERT(false, "unknown renderer api");

		return nullptr;
	}

	IndexBuffer* IndexBuffer::create(uint32_t* indices, uint32_t count)
	{
		switch (RendererAPI::getCurrentAPIType())
		{
			case RendererAPI::RendererAPIType::None: DN_CORE_ASSERT(false, "RendererAPI::RendererAPIType::None:"); return nullptr;
			case RendererAPI::RendererAPIType::OpenGL: return new OpenGLIndexBuffer(indices, count);
		}
		DN_CORE_ASSERT(false, "unknown renderer api");
		return nullptr;
	}
}