#ifndef OPENGL_BUFFERS_H
#define OPENGL_BUFFERS_H

#include "renderer/buffers.h"
#include <stdint.h>


namespace Donut
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void bind() const override;
		virtual void unBind() const override;

	private:
		uint32_t object_id_;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t size);
		virtual ~OpenGLIndexBuffer();

		virtual void bind() const override;
		virtual void unBind() const override;

		virtual uint32_t getIndicesCount() const override;
	private:
		uint32_t object_id_;
		uint32_t indices_count_;
	};
}
#endif