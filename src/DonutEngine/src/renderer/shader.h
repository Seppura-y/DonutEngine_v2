#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glm/glm.hpp>

namespace Donut
{
	class Shader
	{
	public:
		Shader() = default;
		virtual ~Shader() {}

		virtual const std::string& getName() = 0;

		static Ref<Shader> createShader(const std::string& filepath);
		static Ref<Shader> createShader(const std::string& name, const std::string& vertex_string, const std::string& frag_string);

		virtual void bind() const = 0;
		virtual void unBind() const = 0;


		virtual void setInt(const std::string& name, uint32_t value) = 0;
		virtual void setFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void setFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void setMat4(const std::string& name, const glm::mat4& value) = 0;
	private:
		unsigned int shader_id_;
	};


	class ShaderLibrary
	{
	public:
		void add(const std::string& name, const Ref<Shader>& shader);
		void add(const Ref<Shader>& shader);
		Ref<Shader> load(const std::string& filepath);
		Ref<Shader> load(const std::string& name, const std::string& filepath);

		Ref<Shader> get(const std::string& name);

		bool exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> shaders_;
	};
}

#endif