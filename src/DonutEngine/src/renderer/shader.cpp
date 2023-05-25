#include "pch.h"
#include "shader.h"

#include "renderer/renderer_api.h"
#include "platform/opengl/opengl_shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Donut
{
	Shader* Shader::createShader(const std::string& filepath)
	{

		switch (RendererAPI::getApiType())
		{
		case RendererAPI::RendererAPIType::None:    DN_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::RendererAPIType::OpenGL:  return new OpenGLShader(filepath);
		}

		DN_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	

	Shader* Shader::createShader(const std::string& vertex_shader, const std::string& fragment_shader)
	{
		switch (RendererAPI::getApiType())
		{
			case RendererAPI::RendererAPIType::None:
			{
				DN_CORE_WARN("RendererAPI::None");
				return nullptr;
			}
			case RendererAPI::RendererAPIType::OpenGL:
			{
				return new OpenGLShader(vertex_shader, fragment_shader);
			}
		}

		DN_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
}