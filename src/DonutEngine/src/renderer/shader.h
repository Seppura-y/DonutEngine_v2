#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <glm/glm.hpp>

namespace Donut
{
	class Shader
	{
	public:
		Shader(const std::string& vertex_string, const std::string& frag_string);
		~Shader();

		void bind() const;
		void unBind() const;

		void uploadUniformMat4v(const std::string& name, const glm::mat4& matrix);
	private:
		unsigned int shader_id_;
	};
}

#endif