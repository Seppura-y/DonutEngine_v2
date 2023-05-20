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
		virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer) override;
		virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer) override;
		virtual void bind() const override;
		virtual void unBind() const override;
		virtual const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const override { return vertex_buffers_; }
		virtual const std::shared_ptr<IndexBuffer>& getIndexBuffer() const override { return index_buffer_; }
	private:
		unsigned int object_id_;
		std::vector<std::shared_ptr<VertexBuffer>> vertex_buffers_;
		std::shared_ptr<IndexBuffer> index_buffer_;
	};
}
#endif