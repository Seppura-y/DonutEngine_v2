#include "renderer_2d.h"

#include "renderer/vertex_array.h"
#include "renderer/shader.h"

#include "renderer/render_command.h"

#include "platform/opengl/opengl_shader.h"
#include "platform/opengl/opengl_vertex_array.h"
#include "platform/opengl/opengl_texture.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Donut
{
	struct Renderer2DStorage
	{
		Ref<VertexArray> rectangle_va_;
		Ref<Shader> rectangle_shader_;
		Ref<Shader> texture_shader_;
	};

	static Renderer2DStorage* s_data;

	void Renderer2D::init()
	{
		s_data = new Renderer2DStorage();

		s_data->rectangle_va_ = VertexArray::create();

		float rect_vertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		Ref<VertexBuffer> rectangle_vb;
		rectangle_vb.reset(VertexBuffer::create(rect_vertices, sizeof(rect_vertices)));
		rectangle_vb->setLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float2, "a_TexCoord"}
			});
		s_data->rectangle_va_->addVertexBuffer(rectangle_vb);

		uint32_t rectangle_indices[6] = { 0, 1, 2, 2, 3, 0 };
		Ref<IndexBuffer> rectangle_ib;
		rectangle_ib.reset(IndexBuffer::create(rectangle_indices, sizeof(rectangle_indices) / sizeof(uint32_t)));
		s_data->rectangle_va_->setIndexBuffer(rectangle_ib);

		s_data->rectangle_shader_ = Shader::createShader("assets/shaders/flatcolor.glsl");
		s_data->texture_shader_ = Shader::createShader("assets/shaders/texture.glsl");
		s_data->texture_shader_->bind();
		s_data->texture_shader_->setInt("u_texture", 0);
	}

	void Renderer2D::shutdown()
	{
		delete s_data;
	}

	void Renderer2D::beginScene(const OrthographicCamera& camera)
	{
		std::dynamic_pointer_cast<Donut::OpenGLShader>(s_data->rectangle_shader_)->bind();
		std::dynamic_pointer_cast<Donut::OpenGLShader>(s_data->rectangle_shader_)->setMat4("u_viewProjectionMatrix", camera.getViewProjectionMatrix());

		std::dynamic_pointer_cast<Donut::OpenGLShader>(s_data->texture_shader_)->bind();
		std::dynamic_pointer_cast<Donut::OpenGLShader>(s_data->texture_shader_)->setMat4("u_viewProjectionMatrix", camera.getViewProjectionMatrix());
	}

	void Renderer2D::endScene()
	{

	}

	void Renderer2D::drawRectangle(const glm::vec2& position, glm::vec2& size, glm::vec4& color)
	{
		drawRectangle({ position.x, position.y, position.x }, size, color);
	}

	void Renderer2D::drawRectangle(const glm::vec3& position, glm::vec2& size, glm::vec4& color)
	{
		std::dynamic_pointer_cast<Donut::OpenGLShader>(s_data->rectangle_shader_)->bind();

		std::dynamic_pointer_cast<Donut::OpenGLVertexArray>(s_data->rectangle_va_)->bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		std::dynamic_pointer_cast<Donut::OpenGLShader>(s_data->rectangle_shader_)->setMat4("u_transformMatrix", transform);
		std::dynamic_pointer_cast<Donut::OpenGLShader>(s_data->rectangle_shader_)->setFloat4("u_color", color);

		RenderCommand::drawIndices(s_data->rectangle_va_);
	}

	void Renderer2D::drawRectangle(const glm::vec2& position, glm::vec2& size, Ref<Texture2D>& texture)
	{
		drawRectangle(glm::vec3(position.x, position.y, 0.0f), size, texture);
	}

	void Renderer2D::drawRectangle(const glm::vec3& position, glm::vec2& size, Ref<Texture2D>& texture)
	{
		std::dynamic_pointer_cast<Donut::OpenGLShader>(s_data->texture_shader_)->bind();

		std::dynamic_pointer_cast<Donut::OpenGLVertexArray>(s_data->rectangle_va_)->bind();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		std::dynamic_pointer_cast<Donut::OpenGLShader>(s_data->texture_shader_)->setMat4("u_transformMatrix", transform);
		//std::dynamic_pointer_cast<Donut::OpenGLShader>(s_data->texture_shader_)->setInt("u_texture", 0);
		
		std::dynamic_pointer_cast<Donut::OpenGLTexture2D>(texture)->bind();

		RenderCommand::drawIndices(s_data->rectangle_va_);
	}
}