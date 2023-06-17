#ifndef RENDERER_H
#define RENDERER_H

#include "renderer/renderer_api.h"
#include "renderer/render_command.h"
#include "renderer/shader.h"
#include "renderer/orthographic_camera.h"

#include <glm/glm.hpp>

namespace Donut
{
	class Renderer
	{
	public:
		static void init();
		static void onWindowResize(uint32_t width, uint32_t height);
		static void beginScene(OrthographicCamera& camera);
		static void endScene();
		static void submit(const Donut::Ref<Shader>& shader, const Donut::Ref<VertexArray>& va);
		static void submit(const Donut::Ref<Shader>& shader,const Donut::Ref<VertexArray>& va, const glm::mat4& transform);
		inline static RendererAPI::RendererAPIType getRendererApi() { return s_type_; }
	private:

		struct SceneData
		{
			glm::mat4 view_projection_matrix_;
		};

		static SceneData* scene_data_;

		static inline RendererAPI::RendererAPIType s_type_ = RendererAPI::RendererAPIType::OpenGL;

	};
}


#endif