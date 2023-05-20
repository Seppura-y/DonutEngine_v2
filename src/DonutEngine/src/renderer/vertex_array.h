#ifndef VERTEX_ARRAY
#define VERTEX_ARRAY
#include <memory>
#include <vector>

#include "renderer/buffers.h"

namespace Donut
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() {}
		virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer) = 0;
		virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer) = 0;
		virtual void bind() const = 0;
		virtual void unBind() const = 0;
		virtual const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const = 0;
		virtual const std::shared_ptr<IndexBuffer>& getIndexBuffer() const = 0;

		static VertexArray* create();
	private:

	};
}

#endif