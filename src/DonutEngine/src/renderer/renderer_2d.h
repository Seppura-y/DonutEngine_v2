#ifndef RENDERER_2D_H
#define RENDERER_2D_H

#include "renderer/orthographic_camera.h"

namespace Donut
{
	class Renderer2D
	{
	public:
		static void init();
		static void shutdown();

		static void beginScene(const OrthographicCamera& camera);
		static void endScene();

		static void drawRectangle(const glm::vec2& position, glm::vec2& size, glm::vec4& color);
		static void drawRectangle(const glm::vec3& position, glm::vec2& size, glm::vec4& color);
	};
}
#endif