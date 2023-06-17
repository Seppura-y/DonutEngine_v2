#include "pch.h"
#include "opengl_shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "core/timer.h"

namespace Donut
{



	namespace Utils {

		//static GLenum shaderTypeFromString(const std::string& type)
		//{
		//	if (type == "vertex")
		//		return GL_VERTEX_SHADER;
		//	if (type == "fragment" || type == "pixel")
		//		return GL_FRAGMENT_SHADER;

		//	DN_CORE_ASSERT(false, "Unknown shader type!");
		//	return 0;
		//}

		static GLenum getShaderType(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;

			DN_CORE_ASSERT(false, "Unknown shader type!");
			return 0;
		}

		static shaderc_shader_kind glShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
			}
			DN_CORE_ASSERT(false, "");
			return (shaderc_shader_kind)0;
		}

		static const char* glShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
			case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			}
			DN_CORE_ASSERT(false, "");
			return nullptr;
		}

		static const char* getCacheDirectory()
		{
			// TODO: make sure the assets directory is valid
			return "assets/cache/shader/opengl";
		}

		static void createCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = getCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static const char* glShaderStageCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
			}
			DN_CORE_ASSERT(false, "");
			return "";
		}

		static const char* glShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
			}
			DN_CORE_ASSERT(false, "");
			return "";
		}


	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: filepath_(filepath)
	{
		DN_PROFILE_FUNCTION();

		Utils::createCacheDirectoryIfNeeded();

		std::string source = readFile(filepath);
		auto shader_sources = preProcess(source);
		//compileShaders(shader_sources);

		{
			Timer timer;
			compileOrGetVulkanBinaries(shader_sources);
			compileOrGetOpenGLBinaries();
			createProgram();
			DN_CORE_WARN("Shader create took {0} ms", timer.elapsedMillis());
		}
	
		auto last_slash = filepath.find_last_of("/\\");
		last_slash = last_slash == std::string::npos ? 0 : last_slash + 1;
		auto last_dot = filepath.rfind('.');
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
		//compileShaders(sources);

		compileOrGetVulkanBinaries(sources);
		compileOrGetOpenGLBinaries();
		createProgram();
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
			DN_CORE_ASSERT(Utils::getShaderType(type), "Invalid shader type specified");

			size_t next_line_pos = source.find_first_not_of("\r\n", eol);
			pos = source.find(type_token, next_line_pos);
			shader_sources[Utils::getShaderType(type)] = (pos == std::string::npos) ?
				source.substr(next_line_pos) : source.substr(next_line_pos, pos - next_line_pos);
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

	void OpenGLShader::compileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shader_sources)
	{
		GLuint program = glCreateProgram();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
		const bool optimize = true;
		if (optimize)
		{
			options.SetOptimizationLevel(shaderc_optimization_level_performance);
		}

		std::filesystem::path cache_dir = Utils::getCacheDirectory();

		auto& shader_data = vulkan_spirv_;
		shader_data.clear();
		for (auto&& [stage, source] : shader_sources)
		{
			std::filesystem::path shader_filepath = filepath_;
			std::filesystem::path cache_path = cache_dir / (shader_filepath.filename().string() + Utils::glShaderStageCachedVulkanFileExtension(stage));
			std::ifstream in(cache_path, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shader_data[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::glShaderStageToShaderC(stage), filepath_.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					DN_CORE_ERROR(module.GetErrorMessage());
					DN_CORE_ASSERT(false, "CompileGlslToSpv failed!");
				}

				shader_data[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cache_path, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shader_data[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data] : shader_data)
		{
			reflect(stage, data);
		}
	}

	void OpenGLShader::compileOrGetOpenGLBinaries()
	{
		auto& shader_data = opengl_spirv_;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize)
		{
			options.SetOptimizationLevel(shaderc_optimization_level_performance);
		}

		std::filesystem::path cache_dir = Utils::getCacheDirectory();

		shader_data.clear();
		opengl_source_codes_.clear();
		for (auto&& [stage, spirv] : vulkan_spirv_)
		{
			std::filesystem::path shader_filepath = filepath_;
			std::filesystem::path cache_path = cache_dir / (shader_filepath.filename().string() + Utils::glShaderStageCachedOpenGLFileExtension(stage));

			std::ifstream in(cache_path, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shader_data[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				spirv_cross::CompilerGLSL glsl_compiler(spirv);
				opengl_source_codes_[stage] = { glsl_compiler.compile() };
				auto& source = opengl_source_codes_[stage];

				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::glShaderStageToShaderC(stage), filepath_.c_str());
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					DN_CORE_ERROR(module.GetErrorMessage());
					DN_CORE_ASSERT(false, "");
				}

				shader_data[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
				std::ofstream out(cache_path, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shader_data[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}

	void OpenGLShader::createProgram()
	{
		GLuint program = glCreateProgram();

		std::vector<GLuint> shader_ids;
		for (auto&& [stage, spirv] : opengl_spirv_)
		{
			GLuint shader_id = shader_ids.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shader_id, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
			glSpecializeShader(shader_id, "main", 0, nullptr, nullptr);
			glAttachShader(program, shader_id);
		}

		glLinkProgram(program);

		GLint is_linked;
		glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint max_length;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

			std::vector<GLchar> info_log(max_length);
			glGetProgramInfoLog(program, max_length, &max_length, info_log.data());
			DN_CORE_ERROR("Shader linking failed {0} : \n {1}", filepath_, info_log.data());

			glDeleteProgram(program);

			for (auto id : shader_ids)
			{
				glDeleteShader(id);
			}
		}

		for (auto id : shader_ids)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		shader_id_ = program;
	}

	void OpenGLShader::reflect(GLenum stage, const std::vector<uint32_t>& shader_data)
	{
		spirv_cross::Compiler compiler(shader_data);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		DN_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::glShaderStageToString(stage), filepath_);
		DN_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		DN_CORE_TRACE("    {0} resources", resources.sampled_images.size());

		DN_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();

			DN_CORE_TRACE("  {0}", resource.name);
			DN_CORE_TRACE("    Size = {0}", bufferSize);
			DN_CORE_TRACE("    Binding = {0}", binding);
			DN_CORE_TRACE("    Members = {0}", memberCount);
		}
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

	void OpenGLShader::setIntArray(const std::string& name, int* values, uint32_t count)
	{
		DN_PROFILE_FUNCTION();

		uploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::setFloat(const std::string& name, float value)
	{
		DN_PROFILE_FUNCTION();
		uploadUniformFloat(name, value);
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

	void OpenGLShader::uploadUniformIntArray(const std::string& name, const int* values, uint32_t count)
	{
		DN_PROFILE_FUNCTION();
		GLint location = glGetUniformLocation(shader_id_, name.c_str());
		glUniform1iv(location, count, values);
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