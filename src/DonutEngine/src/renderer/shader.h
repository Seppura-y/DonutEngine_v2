#ifndef SHADER_H
#define SHADER_H

#include <string>
namespace Donut
{
	class Shader
	{
	public:
		Shader(const std::string& vertex_string, const std::string& frag_string);
		~Shader();

		void bind() const;
		void unBind() const;
	private:
		unsigned int shader_id_;
	};
}

#endif