#ifndef RENDERER_API_H
#define RENDERER_API_H

namespace Donut
{
	class RendererAPI
	{
	public:
		enum class RendererAPIType{None = 0, OpenGL = 1};
		inline static RendererAPIType getCurrentAPIType() { return type_; }

	private:
		static RendererAPIType type_;
	};
}
#endif