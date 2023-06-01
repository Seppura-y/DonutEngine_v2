//#include "donut.h"
//
//#include "imgui/imgui.h"
//#include "renderer/graphics_context.h"
//#include "renderer/shader.h"
//
//#include "platform/windows/windows_window.h"
//#include "platform/opengl/opengl_context.h"
//#include "platform/opengl/opengl_shader.h"
//
//#include "platform/opengl/opengl_texture.h"
//
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include "sandbox_2d.h"
//
//#include "core/entry_point.h"
//
////#pragma comment(lib, "DonutEngine2.lib")
//
//class ExampleLayer : public Donut::Layer
//{
//public:
//	ExampleLayer()
//		:	Layer("Example"),
//			camera_controller_((float)1600 / (float)900, true),
//			rect_position_(0.0f)
//	{
//		triangle_va_ = Donut::VertexArray::create();
//
//
//		float vertices[3 * 7] =
//		{
//			-0.5f, -0.5f, 0.0f, 0.8f, 0.5f, 0.3f, 1.0f,
//			 0.5f, -0.5f, 0.0f, 0.8f, 0.5f, 0.3f, 1.0f,
//			 0.0f,  0.5f, 0.0f, 0.8f, 0.5f, 0.3f, 1.0f
//		};
//
//		Donut::Ref<Donut::VertexBuffer> vertex_buffer;
//		vertex_buffer.reset(Donut::VertexBuffer::create(vertices, sizeof(vertices)));
//		{
//			Donut::BufferLayout layout = {
//				{ Donut::ShaderDataType::Float3, "a_Position" },
//				{ Donut::ShaderDataType::Float4, "a_Color"}
//			};
//			vertex_buffer->setLayout(layout);
//		}
//
//		unsigned int indices[3] = { 0, 1, 2 };
//		Donut::Ref<Donut::IndexBuffer> index_buffer;
//		index_buffer.reset(Donut::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
//
//		triangle_va_->addVertexBuffer(vertex_buffer);
//		triangle_va_->setIndexBuffer(index_buffer);
//
//
//		std::string vertex_string = R"(
//			#version 450 core
//			
//			layout(location = 0) in vec3 a_Position;
//			layout(location = 1) in vec4 a_Color;
//
//			uniform mat4 u_viewProjectionMatrix;
//
//			out vec3 v_Position;
//			out vec4 v_Color;
//
//			void main()
//			{
//				v_Position = a_Position;
//				v_Color = a_Color;
//				gl_Position = u_viewProjectionMatrix * vec4(a_Position, 1.0f);
//			}
//		)";
//
//		std::string fragment_string = R"(
//			#version 450 core
//
//			layout(location = 0) out vec4 color;
//
//			in vec3 v_Position;
//			in vec4 v_Color;
//
//			void main()
//			{
//				//color = vec4(v_Position * 0.5 + 0.5, 1.0);
//				color = v_Color;
//			}
//		)";
//
//		triangle_shader_ = Donut::Shader::createShader("triangle shader", vertex_string, fragment_string);
//
//
//
//
//
//
//
//		rectangle_va_ = Donut::VertexArray::create();
//		float rect_vertices[3 * 4] =
//		{
//			-0.5f, -0.5f, 0.0f,
//			 0.5f, -0.5f, 0.0f,
//			 0.5f,  0.5f, 0.0f,
//			-0.5f,  0.5f, 0.0f
//		};
//		Donut::Ref<Donut::VertexBuffer> rect_vb;
//		rect_vb.reset(Donut::VertexBuffer::create(rect_vertices, sizeof(rect_vertices)));
//		Donut::BufferLayout rectangle_layout =
//		{
//			{Donut::ShaderDataType::Float3,"a_Position"}
//		};
//		rect_vb->setLayout(rectangle_layout);
//		rectangle_va_->addVertexBuffer(rect_vb);
//
//		uint32_t rect_indices[] = { 0,1,2,2,3,0 };
//		Donut::Ref<Donut::IndexBuffer> rect_ib;
//		rect_ib.reset(Donut::IndexBuffer::create(rect_indices, sizeof(rect_indices)));
//		rectangle_va_->setIndexBuffer(rect_ib);
//
//		std::string rect_v_src = R"(
//			#version 450 core
//			
//			layout(location = 0) in vec3 a_Position;
//
//			uniform mat4 u_viewProjectionMatrix;
//			uniform mat4 u_transformMatrix;
//
//			void main()
//			{
//				gl_Position = u_viewProjectionMatrix * u_transformMatrix * vec4(a_Position, 1.0);	
//			}
//		)";
//
//		std::string rect_f_src = R"(
//			#version 450 core
//
//			layout(location = 0) out vec4 color;
//
//			in vec4 v_Color;
//
//			uniform vec4 u_Color;
//
//			void main()
//			{
//				color = u_Color;
//			}
//		)";
//
//		rectangle_shader_ = Donut::Shader::createShader("rectangle shader", rect_v_src, rect_f_src);
//
//
//
//
//
//
//		texture_va_ = Donut::VertexArray::create();
//		float texture_vertices[5 * 4] =
//		{
//			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
//			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
//			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
//			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
//		};
//		Donut::Ref<Donut::VertexBuffer> texture_vb;
//		texture_vb.reset(Donut::VertexBuffer::create(texture_vertices, sizeof(texture_vertices)));
//		Donut::BufferLayout texture_layout =
//		{
//			{Donut::ShaderDataType::Float3,"a_Position"},
//			{Donut::ShaderDataType::Float2,"a_TexCoord"}
//		};
//		texture_vb->setLayout(texture_layout);
//		texture_va_->addVertexBuffer(texture_vb);
//
//		uint32_t texture_indices[] = { 0,1,2,2,3,0 };
//		Donut::Ref<Donut::IndexBuffer> texture_ib;
//		texture_ib.reset(Donut::IndexBuffer::create(texture_indices, sizeof(texture_indices)));
//		texture_va_->setIndexBuffer(texture_ib);
//
//		std::string textureShaderVertexSrc = R"(
//			#version 450 core
//			
//			layout(location = 0) in vec3 a_Position;
//			layout(location = 1) in vec2 a_TexCoord;
//
//			uniform mat4 u_viewProjectionMatrix;
//			uniform mat4 u_transformMatrix;
//
//			out vec2 v_TexCoord;
//			void main()
//			{
//				v_TexCoord = a_TexCoord;
//				gl_Position = u_viewProjectionMatrix * u_transformMatrix * vec4(a_Position, 1.0);	
//			}
//		)";
//
//		std::string textureShaderFragmentSrc = R"(
//			#version 450 core
//
//			layout(location = 0) out vec4 color;
//
//			in vec2 v_TexCoord;
//
//			uniform sampler2D u_Texture;
//
//			void main()
//			{
//				color = texture(u_Texture, v_TexCoord);
//			}
//		)";
//
//		shader_library_.load("assets/shaders/texture.glsl");
//		//texture_shader_.reset(Donut::Shader::createShader(textureShaderVertexSrc, textureShaderFragmentSrc));
//
//
//		texture_ = Donut::Texture2D::createTexture("assets/textures/checkbox.png");
//		texture2_ = Donut::Texture2D::createTexture("assets/textures/cherno_logo.png");
//		//texture2_ = Donut::Texture2D::createTexture("assets/textures/blending_transparent_window.png");
//
//	}
//
//	void onUpdate(Donut::Timestep ts) override
//	{
//		//DN_CLIENT_TRACE("Delta Time: {0} s  {1} ms", ts.getSeconds(), ts.getMilliseconds());
//
//		camera_controller_.onUpdate(ts);
//
//
//		if (Donut::Input::isKeyPressed(DN_KEY_R))
//		{
//			rect_rotation_ -= rect_rotate_speed_ * ts;
//		}
//
//		if (Donut::Input::isKeyPressed(DN_KEY_T))
//		{
//			rect_rotation_ += rect_rotate_speed_ * ts;
//		}
//
//		glm::mat4 main_transform = glm::translate(glm::mat4(1.0f), rect_position_);
//		main_transform = glm::rotate(main_transform, glm::radians(rect_rotation_), glm::vec3(0, 0, 1));
//		main_transform = glm::scale(main_transform, glm::vec3(0.1f));
//
//		Donut::RenderCommand::setClearColor({ 0.2, 0.2, 0.2, 1 });
//		Donut::RenderCommand::clear();
//
//		//camera_.setPosition(camera_pos_);
//		//camera_.setRotation(camera_rotation_);
//
//		Donut::Renderer::beginScene(camera_controller_.getCamera());
//
//		glm::vec4 red_color(0.8f, 0.3f, 0.2f, 1.0f);
//		glm::vec4 blue_color(0.2f, 0.3f, 0.8f, 1.0f);
//
//		for (int x = 0; x < 20; x++)
//		{
//			for (int y = 0; y < 20; y++)
//			{
//				//if (x % 2)
//				//{
//				//	if (y % 2)
//				//	{
//				//		//std::dynamic_pointer_cast<Donut::OpenGLShader>(rectangle_shader_)->uploadUniformFloat4("u_Color", blue_color);
//				//		std::dynamic_pointer_cast<Donut::OpenGLShader>(rectangle_shader_)->uploadUniformFloat4("u_Color", rect_color_);
//				//	}
//				//	else
//				//	{
//				//		std::dynamic_pointer_cast<Donut::OpenGLShader>(rectangle_shader_)->uploadUniformFloat4("u_Color", red_color);
//				//	}
//				//}
//				//else
//				//{
//				//	if (y % 2)
//				//	{
//				//		std::dynamic_pointer_cast<Donut::OpenGLShader>(rectangle_shader_)->uploadUniformFloat4("u_Color", red_color);
//				//	}
//				//	else
//				//	{
//				//		std::dynamic_pointer_cast<Donut::OpenGLShader>(rectangle_shader_)->uploadUniformFloat4("u_Color", rect_color_);
//				//		//std::dynamic_pointer_cast<Donut::OpenGLShader>(rectangle_shader_)->uploadUniformFloat4("u_Color", blue_color);
//				//	}
//				//}
//
//				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
//				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos);
//				transform = transform * main_transform;
//				std::dynamic_pointer_cast<Donut::OpenGLShader>(rectangle_shader_)->bind();
//				std::dynamic_pointer_cast<Donut::OpenGLShader>(rectangle_shader_)->uploadUniformFloat4("u_Color", rect_color_);
//				Donut::Renderer::submit(rectangle_shader_, rectangle_va_, transform);
//			}
//		}
//
//		auto texture_shader = shader_library_.get("texture");
//
//		std::dynamic_pointer_cast<Donut::OpenGLShader>(texture_shader)->bind();
//		texture_->bind();
//		Donut::Renderer::submit(texture_shader, texture_va_, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
//		texture2_->bind();
//		Donut::Renderer::submit(texture_shader, texture_va_, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
//
//		Donut::Renderer::endScene();
//
//	}
//
//	virtual void onImGuiRender() override
//	{
//		ImGui::Begin("Settings");
//		ImGui::ColorEdit3("rect color", glm::value_ptr(rect_color_));
//		ImGui::End();
//	}
//
//	void onEvent(Donut::Event& ev) override
//	{
//		camera_controller_.onEvent(ev);
//	}
//
//private:
//	Donut::ShaderLibrary shader_library_;
//	Donut::Ref<Donut::Shader> triangle_shader_;
//	Donut::Ref<Donut::VertexArray> triangle_va_;
//
//	Donut::Ref<Donut::Shader> rectangle_shader_;
//	Donut::Ref<Donut::VertexArray> rectangle_va_;
//
//	//Donut::Ref<Donut::Shader> texture_shader_;
//	Donut::Ref<Donut::VertexArray> texture_va_;
//
//	Donut::Ref<Donut::Texture2D> texture_;
//	Donut::Ref<Donut::Texture2D> texture2_;
//
//	Donut::OrthographicCameraController camera_controller_;
//
//	float rect_move_speed_ = 5.0f;
//	float rect_rotate_speed_ = 50.0f;
//	float rect_rotation_ = 0.0f;
//	glm::vec3 rect_position_{ 0.0f, 0.0f, 0.0f };
//	glm::vec4 rect_color_{ 0.2f, 0.3f, 0.8f, 1.0f };
//};
//
//class Sandbox : public Donut::Application
//{
//public:
//	Sandbox()
//	{
//		//pushLayer(new ExampleLayer());
//		pushLayer(new Sandbox2D());
//	}
//
//	~Sandbox()
//	{
//
//	}
//
//	void windowUpdate() override
//	{
//
//	}
//
//};
//
//Donut::Application* Donut::createApplication()
//{
//	return new Sandbox();
//}
