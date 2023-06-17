#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H
#include "renderer/shader.h"
#include "glm/glm.hpp"


typedef unsigned int GLenum;

namespace Donut
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader() = default;
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertex_shader, const std::string& fragment_shader);
		virtual ~OpenGLShader();

		virtual const std::string& getName() override { return name_; }

		virtual void bind() const override;
		virtual void unBind() const override;

		virtual void setInt(const std::string& name, uint32_t value) override;
		virtual void setIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void setFloat(const std::string& name, float value) override;
		virtual void setFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void setFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void setMat4(const std::string& name, const glm::mat4& value) override;


		void uploadUniformInt(const std::string& name, const int value);
		void uploadUniformIntArray(const std::string& name, const int* values, uint32_t count);

		void uploadUniformFloat(const std::string& name, const float value);
		void uploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void uploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void uploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void uploadUniformMat3fv(const std::string& name, const glm::mat3& matrix);
		void uploadUniformMat4fv(const std::string& name, const glm::mat4& matrix);

	private:
		std::string readFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> preProcess(const std::string& source);
		void compileShaders(const std::unordered_map<GLenum, std::string>& shader_sources);

		void compileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shader_sources);
		void compileOrGetOpenGLBinaries();
		void createProgram();
		void reflect(GLenum stage, const std::vector<uint32_t>& shader_data);
	private:
		unsigned int shader_id_;
		std::string name_;

		std::string filepath_;
		std::unordered_map<GLenum, std::vector<uint32_t>> vulkan_spirv_;
		std::unordered_map<GLenum, std::vector<uint32_t>> opengl_spirv_;

		std::unordered_map<GLenum, std::string> opengl_source_codes_;
	};
}
#endif