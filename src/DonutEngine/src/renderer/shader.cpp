#include "pch.h"
#include "shader.h"

#include "renderer/renderer_api.h"
#include "platform/opengl/opengl_shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Donut
{
	Ref<Shader> Shader::createShader(const std::string& filepath)
	{

		switch (RendererAPI::getApiType())
		{
		case RendererAPI::RendererAPIType::None:    DN_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::RendererAPIType::OpenGL:  return std::make_shared<OpenGLShader>(filepath);
		}

		DN_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	

	Ref<Shader> Shader::createShader(const std::string& name, const std::string& vertex_shader, const std::string& fragment_shader)
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
				return std::make_shared<OpenGLShader>(name, vertex_shader, fragment_shader);
			}
		}

		DN_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}

	void ShaderLibrary::add(const std::string& name, const Ref<Shader>& shader)
	{
		DN_CORE_ASSERT(!exists(name), "Shader already exists!");
		shaders_[name] = shader;
	}

	void ShaderLibrary::add(const Ref<Shader>& shader)
	{
		auto& name = shader->getName();
		DN_CORE_ASSERT(shaders_.find(name) == shaders_.end(), "Shader already exists!");
		shaders_[name] = shader;
	}

	Ref<Shader> ShaderLibrary::load(const std::string& filepath)
	{
		auto shader = Shader::createShader(filepath);
		add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::createShader(filepath);
		add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::get(const std::string& name)
	{
		DN_CORE_ASSERT(exists(name), "Shader does not exist!");
		return shaders_[name];
	}

	bool ShaderLibrary::exists(const std::string& name) const
	{
		return shaders_.find(name) != shaders_.end();
	}
}