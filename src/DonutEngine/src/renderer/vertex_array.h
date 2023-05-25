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
		virtual void addVertexBuffer(const Donut::Ref<VertexBuffer>& buffer) = 0;
		virtual void setIndexBuffer(const Donut::Ref<IndexBuffer>& buffer) = 0;
		virtual void bind() const = 0;
		virtual void unBind() const = 0;
		virtual const std::vector<Donut::Ref<VertexBuffer>>& getVertexBuffers() const = 0;
		virtual const Donut::Ref<IndexBuffer>& getIndexBuffer() const = 0;

		static Ref<VertexArray> create();
	private:

	};
}

#endif