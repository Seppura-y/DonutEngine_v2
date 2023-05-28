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
		//Ref<Shader> rectangle_shader_;
		//Ref<Shader> texture_shader_;
		Ref<Shader> single_shader_;
		Ref<Texture2D> white_texture_;
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

		s_data->white_texture_ = Texture2D::createTexture(1, 1);
		uint32_t white_texture_data = 0xffffffff;
		s_data->white_texture_->setData(&white_texture_data, sizeof(uint32_t));

		s_data->single_shader_ = Shader::createShader("assets/shaders/c3_single_shader.glsl");
		s_data->single_shader_->bind();
		s_data->single_shader_->setInt("u_texture", 0);
	}

	void Renderer2D::shutdown()
	{
		delete s_data;
	}

	void Renderer2D::beginScene(const OrthographicCamera& camera)
	{
		std::dynamic_pointer_cast<Donut::OpenGLShader>(s_data->single_shader_)->bind();
		std::dynamic_pointer_cast<Donut::OpenGLShader>(s_data->single_shader_)->setMat4("u_viewProjectionMatrix", camera.getViewProjectionMatrix());

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
		DN_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_data->single_shader_->setMat4("u_transformMatrix", transform);
		s_data->single_shader_->setFloat4("u_color", color);

		s_data->white_texture_->bind();
		s_data->rectangle_va_->bind();

		RenderCommand::drawIndices(s_data->rectangle_va_);
	}

	void Renderer2D::drawRectangle(const glm::vec2& position, glm::vec2& size, Ref<Texture2D>& texture, float tiling_factor, glm::vec4 tincolor)
	{		DN_PROFILE_FUNCTION();

		drawRectangle(glm::vec3(position.x, position.y, 0.0f), size, texture, tiling_factor, tincolor);
	}

	void Renderer2D::drawRectangle(const glm::vec3& position, glm::vec2& size, Ref<Texture2D>& texture, float tiling_factor, glm::vec4 tincolor)
	{
		DN_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_data->single_shader_->setMat4("u_transformMatrix", transform);
		s_data->single_shader_->setFloat4("u_color", tincolor);
		s_data->single_shader_->setFloat("u_tiling_factor", tiling_factor);

		texture->bind();
		s_data->rectangle_va_->bind();

		RenderCommand::drawIndices(s_data->rectangle_va_);
	}

	void Renderer2D::drawRotatedRectangle(const glm::vec2& position, glm::vec2& size, float rotation, glm::vec4& color)
	{
		drawRotatedRectangle({ position.x, position.y, 1.0f }, size, rotation, color);
	}

	void Renderer2D::drawRotatedRectangle(const glm::vec3& position, glm::vec2& size, float rotation, glm::vec4& color)
	{
		DN_PROFILE_FUNCTION();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation), {0, 0, 1.0f}) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_data->single_shader_->setMat4("u_transformMatrix", transform);
		s_data->single_shader_->setFloat4("u_color", color);

		s_data->white_texture_->bind();
		s_data->rectangle_va_->bind();

		RenderCommand::drawIndices(s_data->rectangle_va_);
	}

	void Renderer2D::drawRotatedRectangle(const glm::vec2& position, glm::vec2& size, float rotation, Ref<Texture2D>& texture, float tiling_factor, glm::vec4 tincolor)
	{
		drawRotatedRectangle({ position.x, position.y, 1.0f }, size, rotation, texture,tiling_factor, tincolor);
	}

	void Renderer2D::drawRotatedRectangle(const glm::vec3& position, glm::vec2& size, float rotation, Ref<Texture2D>& texture, float tiling_factor, glm::vec4 tincolor)
	{
		DN_PROFILE_FUNCTION();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation), {0, 0, 1.0f}) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_data->single_shader_->setMat4("u_transformMatrix", transform);
		s_data->single_shader_->setFloat("u_tiling_factor", tiling_factor);
		s_data->single_shader_->setFloat4("u_color", tincolor);

		texture->bind();
		s_data->rectangle_va_->bind();

		RenderCommand::drawIndices(s_data->rectangle_va_);
	}


}