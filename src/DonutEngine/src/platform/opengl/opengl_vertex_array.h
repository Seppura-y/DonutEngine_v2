#ifndef OPENGL_VERTEX_ARRAY_H
#define OPENGL_VERTEX_ARRAY_H

#include "renderer/vertex_array.h"

namespace Donut
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void addVertexBuffer(const Donut::Ref<VertexBuffer>& buffer) override;
		virtual void setIndexBuffer(const Donut::Ref<IndexBuffer>& buffer) override;

		virtual void bind() const override;
		virtual void unBind() const override;

		virtual const std::vector<Donut::Ref<VertexBuffer>>& getVertexBuffers() const override { return vertex_buffers_; }
		virtual const Donut::Ref<IndexBuffer>& getIndexBuffer() const override { return index_buffer_; }
	private:
		unsigned int object_id_;
		unsigned int vertex_buffer_index_ = 0;
		std::vector<Donut::Ref<VertexBuffer>> vertex_buffers_;
		Donut::Ref<IndexBuffer> index_buffer_;
	};
}
#endif