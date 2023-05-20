#include "pch.h"
#include "shader.h"

#include <glad/glad.h>

namespace Donut
{
	Shader::Shader(const std::string& vertex_string, const std::string& fragment_string)
	{
		GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = vertex_string.c_str();
		glShaderSource(vertex_shader, 1, &source, 0);

		glCompileShader(vertex_shader);

		GLint isCompiled = 0;
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint max_length = 0;
			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &max_length);

			// The max_length includes the NULL character
			std::vector<GLchar> info_log(max_length);
			glGetShaderInfoLog(vertex_shader, max_length, &max_length, &info_log[0]);

			glDeleteShader(vertex_shader);

			DN_CORE_ERROR("{0}", info_log.data());
			DN_CORE_ASSERT(false, "Vertex shader compilation failure!");
			return;
		}

		GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

		// Note that std::string's .c_str is NULL character terminated.
		source = fragment_string.c_str();
		glShaderSource(fragment_shader, 1, &source, 0);

		glCompileShader(fragment_shader);

		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint max_length = 0;
			glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &max_length);

			// The max_length includes the NULL character
			std::vector<GLchar> info_log(max_length);
			glGetShaderInfoLog(fragment_shader, max_length, &max_length, &info_log[0]);

			glDeleteShader(fragment_shader);
			glDeleteShader(vertex_shader);

			DN_CORE_ERROR("{0}", info_log.data());
			DN_CORE_ASSERT(false, "Fragment shader compilation failure!");
			return;
		}

		shader_id_ = glCreateProgram();
		GLuint program = shader_id_;

		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);

		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint max_length = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

			// The max_length includes the NULL character
			std::vector<GLchar> info_log(max_length);
			glGetProgramInfoLog(program, max_length, &max_length, &info_log[0]);

			glDeleteProgram(program);

			glDeleteShader(vertex_shader);
			glDeleteShader(fragment_shader);

			DN_CORE_ERROR("{0}", info_log.data());
			DN_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(program, vertex_shader);
		glDetachShader(program, fragment_shader);
	}

	Shader::~Shader()
	{
		glDeleteProgram(shader_id_);
	}

	void Shader::bind() const
	{
		glUseProgram(shader_id_);
	}

	void Shader::unBind() const
	{
		glUseProgram(0);
	}
}