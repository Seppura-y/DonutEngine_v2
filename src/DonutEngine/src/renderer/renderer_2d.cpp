#include "renderer_2d.h"

#include "renderer/vertex_array.h"
#include "renderer/shader.h"

#include "renderer/render_command.h"

#include "renderer/uniform_buffer.h"

#include "platform/opengl/opengl_shader.h"
#include "platform/opengl/opengl_vertex_array.h"
#include "platform/opengl/opengl_texture.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Donut
{
	struct RectangleVertex
	{
		glm::vec3 position_;
		glm::vec4 color_;
		glm::vec2 tex_coordinate_;

		float texture_index_;
		float tiling_factor_;

		int entity_id_;
	};

	struct Renderer2DData
	{
		static const uint32_t max_rects_ = 20000;
		static const uint32_t max_vertices_ = max_rects_ * 4;
		static const uint32_t max_indices_ = max_rects_ * 6;
		static const uint32_t max_texture_slots_ = 32;

		Ref<VertexArray> rectangle_va_;
		Ref<VertexBuffer> rectangle_vb_;

		Ref<Texture2D> white_texture_;

		Ref<Shader> single_shader_;

		uint32_t rect_indices_count_ = 0;
		RectangleVertex* rect_vertex_buffer_base_ = nullptr;
		RectangleVertex* rect_vertex_buffer_ptr_ = nullptr;

		std::array<Ref<Texture2D>, max_texture_slots_> texture_slots_;
		uint32_t texture_index_ = 1;	//slot 0 for white texture

		glm::vec4 rect_vertex_positions_[4];
		Renderer2D::Statistics statistics_;

		struct CameraData
		{
			glm::mat4 view_projection_;
		};

		CameraData camera_buffer;
		Ref<UniformBuffer> camera_uniform_buffer;
	};

	static Renderer2DData s_data;



	void Renderer2D::init()
	{
		s_data.rectangle_va_ = VertexArray::create();

		s_data.rectangle_vb_.reset(VertexBuffer::create(s_data.max_vertices_ * sizeof(RectangleVertex)));
		s_data.rectangle_vb_->setLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float4, "a_Color"},
				{ ShaderDataType::Float2, "a_TexCoord"},
				{ ShaderDataType::Float,  "a_TexIndex"},
				{ ShaderDataType::Float,  "a_TilingFactor"},
				{ ShaderDataType::Int,  "a_EntityID"}
			});
		s_data.rectangle_va_->addVertexBuffer(s_data.rectangle_vb_);

		s_data.rect_vertex_buffer_base_ = new RectangleVertex[s_data.max_vertices_];

		uint32_t* rectangle_indices = new uint32_t[s_data.max_indices_];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_data.max_indices_; i += 6)
		{
			rectangle_indices[i + 0] = offset + 0;
			rectangle_indices[i + 1] = offset + 1;
			rectangle_indices[i + 2] = offset + 2;

			rectangle_indices[i + 3] = offset + 2;
			rectangle_indices[i + 4] = offset + 3;
			rectangle_indices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> rectangle_ib;
		rectangle_ib.reset(IndexBuffer::create(rectangle_indices, s_data.max_indices_));
		s_data.rectangle_va_->setIndexBuffer(rectangle_ib);
		delete[] rectangle_indices;

		s_data.white_texture_ = Texture2D::createTexture(1, 1);
		uint32_t white_texture_data = 0xffffffff;
		s_data.white_texture_->setData(&white_texture_data, sizeof(uint32_t));

		int samplers[s_data.max_texture_slots_];
		for (uint32_t i = 0; i < s_data.max_texture_slots_; i++)
		{
			samplers[i] = i;
		}

		s_data.single_shader_ = Shader::createShader("assets/shaders/c7_spirv_shader.glsl");
		//s_data.single_shader_ = Shader::createShader("assets/shaders/c6_batch_texture_rendering_v2.glsl");
		//s_data.single_shader_->bind();
		//s_data.single_shader_->setIntArray("u_textures", samplers, s_data.max_texture_slots_);

		s_data.texture_slots_[0] = s_data.white_texture_;

		s_data.rect_vertex_positions_[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_data.rect_vertex_positions_[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_data.rect_vertex_positions_[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_data.rect_vertex_positions_[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_data.camera_uniform_buffer = UniformBuffer::create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::shutdown()
	{
		delete[] s_data.rect_vertex_buffer_base_;
	}

	void Renderer2D::beginScene(const Camera& camera, const glm::mat4& transform)
	{
		DN_PROFILE_FUNCTION();

		//glm::mat4 view_projection = camera.getProjection() * glm::inverse(transform);

		//s_data.single_shader_->bind();
		//s_data.single_shader_->setMat4("u_viewProjectionMatrix", view_projection);

		s_data.camera_buffer.view_projection_ = camera.getProjection() * glm::inverse(transform);
		s_data.camera_uniform_buffer->setData(&s_data.camera_buffer, sizeof(Renderer2DData::CameraData));

		s_data.rect_indices_count_ = 0;
		s_data.rect_vertex_buffer_ptr_ = s_data.rect_vertex_buffer_base_;

		s_data.texture_index_ = 1;
	}

	void Renderer2D::beginScene(const OrthographicCamera& camera)
	{
		s_data.single_shader_->bind();
		s_data.single_shader_->setMat4("u_viewProjectionMatrix", camera.getViewProjectionMatrix());

		s_data.rect_indices_count_ = 0;
		s_data.rect_vertex_buffer_ptr_ = s_data.rect_vertex_buffer_base_;

		s_data.texture_index_ = 1;
	}

	void Renderer2D::beginScene(const EditorCamera& camera)
	{
		DN_PROFILE_FUNCTION();

		//glm::mat4 view_projection = camera.getViewProjection();

		//s_data.single_shader_->bind();
		//s_data.single_shader_->setMat4("u_viewProjectionMatrix", view_projection);

		s_data.camera_buffer.view_projection_ = camera.getViewProjection();
		s_data.camera_uniform_buffer->setData(&s_data.camera_buffer, sizeof(Renderer2DData::CameraData));

		s_data.rect_indices_count_ = 0;
		s_data.rect_vertex_buffer_ptr_ = s_data.rect_vertex_buffer_base_;

		s_data.texture_index_ = 1;
	}

	void Renderer2D::endScene()
	{
		DN_PROFILE_FUNCTION();


		flush();
	}

	void Renderer2D::flush()
	{
		if (s_data.rect_indices_count_ == 0)
		{
			return;
		}

		uint32_t data_size = (uint32_t)((uint8_t*)s_data.rect_vertex_buffer_ptr_ - (uint8_t*)s_data.rect_vertex_buffer_base_);
		s_data.rectangle_vb_->setData(s_data.rect_vertex_buffer_base_, data_size);


		// bind textures
		for (uint32_t i = 0; i < s_data.texture_index_; i++)
		{
			s_data.texture_slots_[i]->bind(i);
		}
		s_data.single_shader_->bind();
		RenderCommand::drawIndices(s_data.rectangle_va_, s_data.rect_indices_count_);
		s_data.statistics_.drawcalls_++;
	}


	void Renderer2D::flushAndReset()
	{
		endScene();
		s_data.rect_indices_count_ = 0;
		s_data.rect_vertex_buffer_ptr_ = s_data.rect_vertex_buffer_base_;
		s_data.texture_index_ = 1;
	}

	void Renderer2D::drawRectangle(const glm::vec2& position, glm::vec2& size, glm::vec4& color)
	{
		drawRectangle({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::drawRectangle(const glm::vec3& position, glm::vec2& size, glm::vec4& color)
	{
		DN_PROFILE_FUNCTION();

		if (s_data.rect_indices_count_ >= Renderer2DData::max_indices_)
		{
			flushAndReset();
		}

		const float tiling_factor = 1.0f;
		constexpr size_t rect_vertex_count = 4;
		float texture_index = 0.0f;

		constexpr glm::vec2 texture_coords[] = { { 0.0f, 0.0f },{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 1.0f } };

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < rect_vertex_count; i++)
		{
			s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[i];
			s_data.rect_vertex_buffer_ptr_->color_ = color;
			s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = texture_coords[i];
			s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
			s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;

			s_data.rect_vertex_buffer_ptr_++;
		}

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[0];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 0.0f, 0.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[1];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 1.0f, 0.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[2];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 1.0f, 1.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[3];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 0.0f, 1.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		s_data.rect_indices_count_ += 6;

		s_data.statistics_.rect_count_++;
	}

	void Renderer2D::drawRectangle(const glm::vec2& position, glm::vec2& size, Ref<Texture2D>& texture, float tiling_factor, glm::vec4 tincolor)
	{
		DN_PROFILE_FUNCTION();

		drawRectangle(glm::vec3(position.x, position.y, 0.0f), size, texture, tiling_factor, tincolor);
	}

	void Renderer2D::drawRectangle(const glm::vec3& position, glm::vec2& size, Ref<Texture2D>& texture, float tiling_factor, glm::vec4 tintcolor)
	{
		DN_PROFILE_FUNCTION();

		if (s_data.rect_indices_count_ >= Renderer2DData::max_indices_)
		{
			flushAndReset();
		}

		constexpr size_t rect_vertex_count = 4;
		float texture_index = 0.0f;

		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		constexpr glm::vec2 texture_coords[] = { { 0.0f, 0.0f },{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 1.0f } };

		for (uint32_t i = 1; i < s_data.texture_index_; i++)
		{
			// 获取raw指针，然后解引用，以调用operator==(const Texture& other)
			if (*s_data.texture_slots_[i].get() == *texture.get())
			{
				texture_index = (float)i;
				break;
			}
		}

		if (texture_index == 0.0f)
		{
			if (s_data.texture_index_ >= Renderer2DData::max_texture_slots_)
			{
				flushAndReset();
			}
			texture_index = (float)s_data.texture_index_;
			s_data.texture_slots_[s_data.texture_index_] = texture;
			s_data.texture_index_++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < rect_vertex_count; i++)
		{
			s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[i];
			s_data.rect_vertex_buffer_ptr_->color_ = color;
			s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = texture_coords[i];
			s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
			s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;

			s_data.rect_vertex_buffer_ptr_++;
		}

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[0];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 0.0f, 0.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[1];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 1.0f, 0.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[2];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 1.0f, 1.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[3];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 0.0f, 1.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		s_data.rect_indices_count_ += 6;

		s_data.statistics_.rect_count_++;

#if 0
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
			glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		s_data.single_shader_->setMat4("u_transformMatrix", transform);
		s_data.single_shader_->setFloat4("u_color", tintcolor);
		s_data.single_shader_->setFloat("u_tiling_factor", tiling_factor);

		texture->bind();
		s_data.rectangle_va_->bind();

		RenderCommand::drawIndices(s_data.rectangle_va_);
#endif
	}

	void Renderer2D::drawRotatedRectangle(const glm::vec2& position, glm::vec2& size, float rotation, glm::vec4& color)
	{
		drawRotatedRectangle({ position.x, position.y, 1.0f }, size, rotation, color);
	}

	void Renderer2D::drawRotatedRectangle(const glm::vec3& position, glm::vec2& size, float rotation, glm::vec4& color)
	{
		DN_PROFILE_FUNCTION();

		if (s_data.rect_indices_count_ >= Renderer2DData::max_indices_)
		{
			flushAndReset();
		}

		const float tiling_factor = 1.0f;
		const float texture_index = 0.0f;

		constexpr size_t rect_vertex_count = 4;

		constexpr glm::vec2 texture_coords[] = { { 0.0f, 0.0f },{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 1.0f } };

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0, 0, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < rect_vertex_count; i++)
		{
			s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[i];
			s_data.rect_vertex_buffer_ptr_->color_ = color;
			s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = texture_coords[i];
			s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
			s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;

			s_data.rect_vertex_buffer_ptr_++;
		}

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[0];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 0.0f, 0.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[1];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 1.0f, 0.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[2];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 1.0f, 1.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[3];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 0.0f, 1.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		s_data.rect_vertex_buffer_ptr_++;

		s_data.rect_indices_count_ += 6;
		s_data.statistics_.rect_count_++;
	}

	void Renderer2D::drawRotatedRectangle(const glm::vec2& position, glm::vec2& size, float rotation, Ref<Texture2D>& texture, float tiling_factor, glm::vec4 tincolor)
	{
		drawRotatedRectangle({ position.x, position.y, 1.0f }, size, rotation, texture,tiling_factor, tincolor);
	}

	void Renderer2D::drawRotatedRectangle(const glm::vec3& position, glm::vec2& size, float rotation, Ref<Texture2D>& texture, float tiling_factor, glm::vec4 tincolor)
	{
		DN_PROFILE_FUNCTION();

		if (s_data.rect_indices_count_ >= Renderer2DData::max_indices_)
		{
			flushAndReset();
		}

		constexpr size_t rect_vertex_count = 4;
		float texture_index = 0.0f;

		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		constexpr glm::vec2 texture_coords[] = { { 0.0f, 0.0f },{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 1.0f } };


		for (uint32_t i = 1; i < s_data.texture_index_; i++)
		{
			// 获取raw指针，然后解引用，以调用operator==(const Texture& other)
			if (*s_data.texture_slots_[i].get() == *texture.get())
			{
				texture_index = (float)i;
				break;
			}
		}

		if (texture_index == 0.0f)
		{
			if (s_data.texture_index_ >= Renderer2DData::max_texture_slots_)
			{
				flushAndReset();
			}
			texture_index = (float)s_data.texture_index_;
			s_data.texture_slots_[s_data.texture_index_] = texture;
			s_data.texture_index_++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0, 0, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < rect_vertex_count; i++)
		{
			s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[i];
			s_data.rect_vertex_buffer_ptr_->color_ = color;
			s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = texture_coords[i];
			s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
			s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;

			s_data.rect_vertex_buffer_ptr_++;
		}

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[0];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 0.0f, 0.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[1];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 1.0f, 0.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[2];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 1.0f, 1.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[3];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = { 0.0f, 1.0f };
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		s_data.rect_indices_count_ += 6;

		s_data.statistics_.rect_count_++;
	}

	void Renderer2D::drawRectangle(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Subtexture>& subtexture, float tiling_factor, glm::vec4& tincolor)
	{
		drawRectangle({ position.x, position.y, 1.0f }, size, rotation, subtexture, tiling_factor, tincolor);
	}

	void Renderer2D::drawRectangle(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Subtexture>& subtexture, float tiling_factor, glm::vec4& tincolor)
	{
		DN_PROFILE_FUNCTION();

		if (s_data.rect_indices_count_ >= Renderer2DData::max_indices_)
		{
			flushAndReset();
		}

		constexpr size_t rect_vertex_count = 4;
		float texture_index = 0.0f;

		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		const glm::vec2* tex_coord = subtexture->getTextureCoord();
		Ref<Texture2D> texture = subtexture->getTexture();

		for (uint32_t i = 1; i < s_data.texture_index_; i++)
		{
			// 获取raw指针，然后解引用，以调用operator==(const Texture& other)
			if (*s_data.texture_slots_[i].get() == *texture.get())
			{
				texture_index = (float)i;
				break;
			}
		}

		if (texture_index == 0.0f)
		{
			if (s_data.texture_index_ >= Renderer2DData::max_texture_slots_)
			{
				flushAndReset();
			}
			texture_index = (float)s_data.texture_index_;
			s_data.texture_slots_[s_data.texture_index_] = texture;
			s_data.texture_index_++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0, 0, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		for (size_t i = 0; i < rect_vertex_count; i++)
		{
			s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[i];
			s_data.rect_vertex_buffer_ptr_->color_ = color;
			s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = tex_coord[i];
			s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
			s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;

			s_data.rect_vertex_buffer_ptr_++;
		}

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[0];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = tex_coord[0];
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[1];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = tex_coord[1];
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[2];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = tex_coord[2];
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		//s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[3];
		//s_data.rect_vertex_buffer_ptr_->color_ = color;
		//s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = tex_coord[3];
		//s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
		//s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
		//s_data.rect_vertex_buffer_ptr_++;

		s_data.rect_indices_count_ += 6;

		s_data.statistics_.rect_count_++;
	}

	void Renderer2D::drawRectangle(const glm::mat4 transform, const glm::vec4& color, int entity_id)
	{
		DN_PROFILE_FUNCTION();

		constexpr size_t rect_vertex_count = 4;
		const float texture_index = 0.0f;
		constexpr glm::vec2 texture_coords[] = { { 0.0f, 0.0f },{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 1.0f } };
		const float tiling_factor = 1.0f;

		if (s_data.rect_indices_count_ >= Renderer2DData::max_indices_)
		{
			flushAndReset();
		}

		for (size_t i = 0; i < rect_vertex_count; i++)
		{
			s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[i];
			s_data.rect_vertex_buffer_ptr_->color_ = color;
			s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = texture_coords[i];
			s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
			s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
			s_data.rect_vertex_buffer_ptr_->entity_id_ = entity_id;
			
			s_data.rect_vertex_buffer_ptr_++;
		}
		s_data.rect_indices_count_ += 6;
		s_data.statistics_.rect_count_++;
	}

	void Renderer2D::drawRectangle(const glm::mat4 transform, const Ref<Texture2D>& texture, float tiling_factor, const glm::vec4& tint_color, int entity_id)
	{
		DN_PROFILE_FUNCTION();

		if (s_data.rect_indices_count_ >= Renderer2DData::max_indices_)
		{
			flushAndReset();
		}
		constexpr size_t rect_vertex_count = 4;
		constexpr glm::vec2 texture_coords[] = { { 0.0f, 0.0f },{ 1.0f, 0.0f },{ 1.0f, 1.0f },{ 0.0f, 1.0f } };

		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		float texture_index = 0.0f;

		for (uint32_t i = 1; i < s_data.texture_index_; i++)
		{
			// 获取raw指针，然后解引用，以调用operator==(const Texture& other)
			if (*s_data.texture_slots_[i].get() == *texture.get())
			{
				texture_index = (float)i;
				break;
			}
		}

		if (texture_index == 0.0f)
		{
			if (s_data.texture_index_ >= Renderer2DData::max_texture_slots_)
			{
				flushAndReset();
			}
			texture_index = (float)s_data.texture_index_;
			s_data.texture_slots_[s_data.texture_index_] = texture;
			s_data.texture_index_++;
		}

		for (size_t i = 0; i < rect_vertex_count; i++)
		{
			s_data.rect_vertex_buffer_ptr_->position_ = transform * s_data.rect_vertex_positions_[i];
			s_data.rect_vertex_buffer_ptr_->color_ = tint_color;
			s_data.rect_vertex_buffer_ptr_->tex_coordinate_ = texture_coords[i];
			s_data.rect_vertex_buffer_ptr_->texture_index_ = texture_index;
			s_data.rect_vertex_buffer_ptr_->tiling_factor_ = tiling_factor;
			s_data.rect_vertex_buffer_ptr_->entity_id_ = entity_id;
			s_data.rect_vertex_buffer_ptr_++;
		}

		s_data.rect_indices_count_ += 6;

		s_data.statistics_.rect_count_++;
	}

	void Renderer2D::drawSprite(const glm::mat4& transform, SpriteRendererComponent& component, int entity_id)
	{
		drawRectangle(transform, component.color_, entity_id);
	}

	void Renderer2D::resetStatistics()
	{
		memset(&s_data.statistics_, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::getStatistics()
	{
		return s_data.statistics_;
	}



}