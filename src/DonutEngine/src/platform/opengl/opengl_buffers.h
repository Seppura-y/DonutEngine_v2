#ifndef OPENGL_BUFFERS_H
#define OPENGL_BUFFERS_H

#include "renderer/buffers.h"
#include <stdint.h>


namespace Donut
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void setLayout(const BufferLayout& layout) override;
		virtual const BufferLayout& getLayout() const override;

		virtual void setData(const void* data, uint32_t size) override;

		virtual void bind() const override;
		virtual void unBind() const override;

	private:
		uint32_t object_id_;
		BufferLayout layout_;
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