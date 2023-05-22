#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H
#include "renderer/shader.h"
#include "glm/glm.hpp"

namespace Donut
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader() = default;
		OpenGLShader(const std::string& vertex_shader, const std::string& fragment_shader);
		virtual ~OpenGLShader();
		virtual void bind() const override;
		virtual void unBind() const override;

		void uploadUniformInt(const std::string& name, const int value);

		void uploadUniformFloat(const std::string& name, const float value);
		void uploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void uploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void uploadUniformFloat4(const std::string& name, const glm::vec4& value);

		void uploadUniformMat3fv(const std::string& name, const glm::mat3& matrix);
		void uploadUniformMat4fv(const std::string& name, const glm::mat4& matrix);
	private:
		unsigned int shader_id_;
	};
}
#endif