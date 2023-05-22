#include "donut.h"

#include "imgui/imgui.h"
#include "renderer/graphics_context.h"
#include "platform/windows/windows_window.h"
#include "platform/opengl/opengl_context.h"

#include <glm/gtc/matrix_transform.hpp>

//#pragma comment(lib, "DonutEngine2.lib")

class ExampleLayer : public Donut::Layer
{
public:
	ExampleLayer() : Layer("Example"), camera_(-1.6f, 1.6f, -0.9f, 0.9f), camera_pos_(0.0f), rect_position_(0.0f)
	{
		triangle_va_.reset(Donut::VertexArray::create());
		//glGenVertexArrays(1, &vao_);
		//glBindVertexArray(vao_);

		float vertices[3 * 7] =
		{
			-0.5f, -0.5f, 0.0f, 0.8f, 0.5f, 0.3f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.8f, 0.5f, 0.3f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.5f, 0.3f, 1.0f
		};

		std::shared_ptr<Donut::VertexBuffer> vertex_buffer;
		vertex_buffer.reset(Donut::VertexBuffer::create(vertices, sizeof(vertices)));
		{
			Donut::BufferLayout layout = {
				{ Donut::ShaderDataType::Float3, "a_Position" },
				{ Donut::ShaderDataType::Float4, "a_Color"}
			};
			vertex_buffer->setLayout(layout);
		}

		unsigned int indices[3] = { 0, 1, 2 };
		std::shared_ptr<Donut::IndexBuffer> index_buffer;
		index_buffer.reset(Donut::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));

		triangle_va_->addVertexBuffer(vertex_buffer);
		triangle_va_->setIndexBuffer(index_buffer);


		std::string vertex_string = R"(
			#version 450 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_viewProjectionMatrix;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_viewProjectionMatrix * vec4(a_Position, 1.0f);
			}
		)";

		std::string fragment_string = R"(
			#version 450 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				//color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		triangle_shader_.reset(new Donut::Shader(vertex_string, fragment_string));







		rectangle_va_.reset(Donut::VertexArray::create());
		float rect_vertices[3 * 4] =
		{
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};
		std::shared_ptr<Donut::VertexBuffer> rect_vb;
		rect_vb.reset(Donut::VertexBuffer::create(rect_vertices, sizeof(rect_vertices)));
		Donut::BufferLayout rectangle_layout =
		{
			{Donut::ShaderDataType::Float3,"a_Position"}
		};
		rect_vb->setLayout(rectangle_layout);
		rectangle_va_->addVertexBuffer(rect_vb);

		uint32_t rect_indices[] = { 0,1,2,2,3,0 };
		std::shared_ptr<Donut::IndexBuffer> rect_ib;
		rect_ib.reset(Donut::IndexBuffer::create(rect_indices, sizeof(rect_indices)));
		rectangle_va_->setIndexBuffer(rect_ib);

		std::string rect_v_src = R"(
			#version 450 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_viewProjectionMatrix;
			uniform mat4 u_transformMatrix;

			out vec3 v_Position;
			void main()
			{
				gl_Position = u_viewProjectionMatrix * u_transformMatrix * vec4(a_Position, 1.0);	
			}
		)";

		std::string rect_f_src = R"(
			#version 450 core

			layout(location = 0) out vec4 color;
			void main()
			{
				color = vec4(0.3,0.3,0.5,1.0);
			}
		)";

		rectangle_shader_.reset(new Donut::Shader(rect_v_src, rect_f_src));
	}

	void onUpdate(Donut::Timestep ts) override
	{
		//DN_CLIENT_TRACE("Delta Time: {0} s  {1} ms", ts.getSeconds(), ts.getMilliseconds());

		if (Donut::Input::isKeyPressed(DN_KEY_LEFT))
		{
			camera_pos_.x += camera_move_speed_ * ts;
		}

		if (Donut::Input::isKeyPressed(DN_KEY_RIGHT))
		{
			camera_pos_.x -= camera_move_speed_ * ts;
		}

		if (Donut::Input::isKeyPressed(DN_KEY_UP))
		{
			camera_pos_.y -= camera_move_speed_ * ts;
		}

		if (Donut::Input::isKeyPressed(DN_KEY_DOWN))
		{
			camera_pos_.y += camera_move_speed_ * ts;
		}

		if (Donut::Input::isKeyPressed(DN_KEY_Q))
		{
			camera_rotation_ -= camera_move_speed_ * ts;
		}

		if (Donut::Input::isKeyPressed(DN_KEY_E))
		{
			camera_rotation_ += camera_move_speed_ * ts;
		}

		if (Donut::Input::isKeyPressed(DN_KEY_A))
		{
			rect_position_.x -= rect_move_speed_ * ts;
		}

		if (Donut::Input::isKeyPressed(DN_KEY_D))
		{
			rect_position_.x += rect_move_speed_ * ts;
		}

		if (Donut::Input::isKeyPressed(DN_KEY_W))
		{
			rect_position_.y -= rect_move_speed_ * ts;
		}

		if (Donut::Input::isKeyPressed(DN_KEY_S))
		{
			rect_position_.y += rect_move_speed_ * ts;
		}

		if (Donut::Input::isKeyPressed(DN_KEY_R))
		{
			rect_rotation_ -= rect_rotate_speed_ * ts;
		}

		if (Donut::Input::isKeyPressed(DN_KEY_T))
		{
			rect_rotation_ += rect_rotate_speed_ * ts;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), rect_position_);
		transform = glm::rotate(transform, glm::radians(rect_rotation_), glm::vec3(0, 0, 1));
		transform = glm::scale(transform, glm::vec3(0.1f));

		Donut::RenderCommand::setClearColor({ 0.2, 0.2, 0.2, 1 });
		Donut::RenderCommand::clear();

		camera_.setPosition(camera_pos_);
		camera_.setRotation(camera_rotation_);

		Donut::Renderer::beginScene(camera_);

		Donut::Renderer::submit(rectangle_shader_, rectangle_va_, transform);
		Donut::Renderer::submit(triangle_shader_, triangle_va_);

		Donut::Renderer::endScene();

	}

	virtual void onImGuiRender() override
	{

	}

	void onEvent(Donut::Event& ev) override
	{
		//Donut::EventDispatcher dispatcher(ev);
		//dispatcher.dispatch<Donut::KeyPressedEvent>(DN_BIND_EVENT_FN(ExampleLayer::onKeyPressedEvent));
	}

	//bool onKeyPressedEvent(Donut::KeyPressedEvent& ev)
	//{
	//	if (ev.getKeyCode() == DN_KEY_LEFT)
	//	{
	//		camera_pos_.x += camera_speed_;
	//	}

	//	if (ev.getKeyCode() == DN_KEY_RIGHT)
	//	{
	//		camera_pos_.x -= camera_speed_;
	//	}

	//	if (ev.getKeyCode() == DN_KEY_UP)
	//	{
	//		camera_pos_.y += camera_speed_;
	//	}

	//	if (ev.getKeyCode() == DN_KEY_DOWN)
	//	{
	//		camera_pos_.y -= camera_speed_;
	//	}

	//	return false;
	//}

private:
	std::shared_ptr<Donut::Shader> triangle_shader_;
	std::shared_ptr<Donut::VertexArray> triangle_va_;

	std::shared_ptr<Donut::Shader> rectangle_shader_;
	std::shared_ptr<Donut::VertexArray> rectangle_va_;

	Donut::OrthographicCamera camera_;
	glm::vec3 camera_pos_;
	float camera_rotation_ = 0.0f;
	float camera_move_speed_ = 1.0f;
	float camera_rotate_speed_ = 10.0f;

	float rect_move_speed_ = 5.0f;
	float rect_rotate_speed_ = 50.0f;
	float rect_rotation_ = 0.0f;
	glm::vec3 rect_position_{ 0.0f, 0.0f, 0.0f };
};

class Sandbox : public Donut::Application
{
public:
	Sandbox()
	{
		pushLayer(new ExampleLayer());
		//pushOverlay(new Donut::ImGuiLayer());
	}

	~Sandbox()
	{

	}

	void windowUpdate() override
	{

	}

};

Donut::Application* Donut::createApplication()
{
	return new Sandbox();
}
