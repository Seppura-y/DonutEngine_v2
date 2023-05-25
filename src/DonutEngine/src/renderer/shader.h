#ifndef SHADER_H
#define SHADER_H

#include <string>

namespace Donut
{
	class Shader
	{
	public:
		Shader() = default;
		virtual ~Shader() {}

		static Shader* createShader(const std::string& filepath);
		static Shader* createShader(const std::string& vertex_string, const std::string& frag_string);

		virtual void bind() const = 0;
		virtual void unBind() const = 0;

	private:
		unsigned int shader_id_;
	};
}

#endif