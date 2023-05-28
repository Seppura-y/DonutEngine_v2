#ifndef RENDERER_2D_H
#define RENDERER_2D_H

#include "renderer/orthographic_camera.h"
#include "renderer/texture.h"

namespace Donut
{
	class Renderer2D
	{
	public:
		static void init();
		static void shutdown();

		static void beginScene(const OrthographicCamera& camera);
		static void endScene();
		static void flush();

		static void drawRectangle(const glm::vec2& position, glm::vec2& size, glm::vec4& color);
		static void drawRectangle(const glm::vec3& position, glm::vec2& size, glm::vec4& color);

		static void drawRotatedRectangle(const glm::vec2& position, glm::vec2& size, float rotation, glm::vec4& color);
		static void drawRotatedRectangle(const glm::vec3& position, glm::vec2& size, float rotation, glm::vec4& color);

		static void drawRectangle(const glm::vec2& position, glm::vec2& size, Ref<Texture2D>& texture, float tiling_factor = 1.0f, glm::vec4 tincolor = glm::vec4(1.0f));
		static void drawRectangle(const glm::vec3& position, glm::vec2& size, Ref<Texture2D>& texture, float tiling_factor = 1.0f, glm::vec4 tincolor = glm::vec4(1.0f));

		static void drawRotatedRectangle(const glm::vec2& position, glm::vec2& size, float rotation, Ref<Texture2D>& texture, float tiling_factor = 1.0f, glm::vec4 tincolor = glm::vec4(1.0f));
		static void drawRotatedRectangle(const glm::vec3& position, glm::vec2& size, float rotation, Ref<Texture2D>& texture, float tiling_factor = 1.0f, glm::vec4 tincolor = glm::vec4(1.0f));
	};
}
#endif