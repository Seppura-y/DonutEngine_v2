#ifndef RENDERER_API_H
#define RENDERER_API_H

#include <glm/glm.hpp>

#include "vertex_array.h"

namespace Donut
{
	class RendererAPI
	{
	public:
		enum class RendererAPIType
		{
			None = 0,
			OpenGL = 1 
		};
		
		inline static RendererAPIType getCurrentAPIType() { return type_; }

		virtual void init() = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void clear() = 0;
		virtual void drawIndices(const Donut::Ref<VertexArray>& vertex_array) = 0;

		inline static RendererAPIType getApiType() { return type_; }
	private:
		static RendererAPIType type_;
	};
}
#endif