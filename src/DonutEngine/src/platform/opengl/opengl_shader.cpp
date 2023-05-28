#include "pch.h"
#include "opengl_shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Donut
{

	static GLenum shaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		DN_CORE_ASSERT(false, "Unknown shader type!");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		DN_PROFILE_FUNCTION();

		std::string source = readFile(filepath);
		auto shader_sources = preProcess(source);
		compileShaders(shader_sources);
	
		auto last_slash = filepath.find_last_of("/\\");
		last_slash = last_slash == std::string::npos ? 0 : last_slash + 1;
		auto last_dot = filepath.rfind(".");
		auto count = last_dot == std::string::npos ? filepath.size() - last_slash : last_dot - last_slash;
		name_ = filepath.substr(last_slash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertex_shader, const std::string& fragment_shader)
		: name_(name)
	{
		DN_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertex_shader;
		sources[GL_FRAGMENT_SHADER] = fragment_shader;
		compileShaders(sources);
	}

	OpenGLShader::~OpenGLShader()
	{
		DN_PROFILE_FUNCTION();

		glDeleteProgram(shader_id_);
	}

	std::string OpenGLShader::readFile(const std::string& filepath)
	{
		DN_PROFILE_FUNCTION();

		std::string result;
		std::ifstream ifs(filepath, std::ios::in | std::ios::binary);
		if (ifs)
		{
			ifs.seekg(0, std::ios::end);
			result.resize(ifs.tellg());
			ifs.seekg(0, std::ios::beg);
			ifs.read(&result[0], result.size());
			ifs.close();
			;
		}
		else
		{
			DN_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::preProcess(const std::string& source)
	{
		DN_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shader_sources;

		const char* type_token = "#type";
		size_t token_length = strlen(type_token);
		size_t pos = source.find(type_token, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			DN_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + token_length + 1;
			std::string type = source.substr(begin, eol - begin);
			DN_CORE_ASSERT(shaderTypeFromString(type), "Invalid shader type specified");

			size_t next_line_pos = source.find_first_not_of("\r\n", eol);
			pos = source.find(type_token, next_line_pos);
			shader_sources[shaderTypeFromString(type)] = source.substr(next_line_pos,
				pos - (next_line_pos == std::string::npos ? source.size() - 1 : next_line_pos));
		}

		return shader_sources;
	}

	void OpenGLShader::compileShaders(const std::unordered_map<GLenum, std::string>& shader_sources)
	{
		DN_PROFILE_FUNCTION();

		GLuint program = glCreateProgram();
		DN_CORE_ASSERT(shader_sources.size() <= 2, "we only support 2 shaders for now");
		std::array<GLenum, 2> glShaderIDs;
		int shader_index = 0;
		//std::vector<GLenum> glShaderIDs(shader_sources.size());
		for (auto& kv : shader_sources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				DN_CORE_ERROR("{0}", infoLog.data());
				DN_CORE_ASSERT(false, "Shader compilation failure!");
				break;
			}

			glAttachShader(program, shader);
			glShaderIDs[shader_index++] = shader;
		}

		shader_id_ = program;

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);

			for (auto id : glShaderIDs)
				glDeleteShader(id);

			DN_CORE_ERROR("{0}", infoLog.data());
			DN_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		for (auto id : glShaderIDs)
			glDetachShader(program, id);
	}

	void OpenGLShader::bind() const
	{
		DN_PROFILE_FUNCTION();

		glUseProgram(shader_id_);
	}

	void OpenGLShader::unBind() const
	{
		DN_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	void OpenGLShader::setInt(const std::string& name, uint32_t value)
	{
		uploadUniformInt(name, value);
	}

	void OpenGLShader::setFloat3(const std::string& name, const glm::vec3& value)
	{
		DN_PROFILE_FUNCTION();

		uploadUniformFloat3(name, value);
	}

	void OpenGLShader::setFloat4(const std::string& name, const glm::vec4& value)
	{
		DN_PROFILE_FUNCTION();

		uploadUniformFloat4(name, value);
	}

	void OpenGLShader::setMat4(const std::string& name, const glm::mat4& value)
	{
		DN_PROFILE_FUNCTION();

		uploadUniformMat4fv(name, value);
	}

	void OpenGLShader::uploadUniformInt(const std::string& name, const int value)
	{
		DN_PROFILE_FUNCTION();

		GLint location = glGetUniformLocation(shader_id_, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::uploadUniformFloat(const std::string& name, const float value)
	{
		DN_PROFILE_FUNCTION();

		GLint location = glGetUniformLocation(shader_id_, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::uploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		DN_PROFILE_FUNCTION();

		GLint location = glGetUniformLocation(shader_id_, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::uploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		DN_PROFILE_FUNCTION();

		GLint location = glGetUniformLocation(shader_id_, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::uploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		DN_PROFILE_FUNCTION();

		GLint location = glGetUniformLocation(shader_id_, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::uploadUniformMat3fv(const std::string& name, const glm::mat3& matrix)
	{
		DN_PROFILE_FUNCTION();

		GLint location = glGetUniformLocation(shader_id_, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::uploadUniformMat4fv(const std::string& name, const glm::mat4& matrix)
	{
		DN_PROFILE_FUNCTION();

		GLint location = glGetUniformLocation(shader_id_, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

}