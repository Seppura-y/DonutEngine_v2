#include "pch.h"

#include <glfw/glfw3.h>
#include <glad/glad.h>

#include <chrono>
#include <thread>

#include "core/application.h"
#include "core/logger.h"
#include "core/input.h"

#include "renderer/renderer.h"

namespace Donut
{
#define BIND_EVENT_FN(x) std::bind(&Application::x,this,std::placeholders::_1)

	Application* Application::s_instance_ = nullptr;

	static GLenum shaderDataTypeToGLenumType(const ShaderDataType& type)
	{
		switch (type)
		{
			case ShaderDataType::Float:		return GL_FLOAT;
			case ShaderDataType::Float2:	return GL_FLOAT;
			case ShaderDataType::Float3:	return GL_FLOAT;
			case ShaderDataType::Float4:	return GL_FLOAT;
			case ShaderDataType::Int:		return GL_INT;
			case ShaderDataType::Int2:		return GL_INT;
			case ShaderDataType::Int3:		return GL_INT;
			case ShaderDataType::Int4:		return GL_INT;
			case ShaderDataType::Bool:		return GL_BOOL;
		}

		DN_CORE_ASSERT(false, "Unknown shader data type!");
		return 0;
	}

	Application::Application()
	{
		s_instance_ = this;
		window_ = std::unique_ptr<Window>(Window::create(WindowProps()));
		window_->setEventCallback(BIND_EVENT_FN(onEvent));

		imgui_layer_ = new ImGuiLayer();
		pushOverlay(imgui_layer_);

		is_running_ = true;

		triangle_va_.reset(VertexArray::create());
		//glGenVertexArrays(1, &vao_);
		//glBindVertexArray(vao_);

		float vertices[3 * 7] =
		{
			-0.5f, -0.5f, 0.0f, 0.8f, 0.5f, 0.3f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.8f, 0.5f, 0.3f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.5f, 0.3f, 1.0f
		};

		std::shared_ptr<VertexBuffer> vertex_buffer;
		vertex_buffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));
		{
			BufferLayout layout = {
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float4, "a_Color"}
			};
			vertex_buffer->setLayout(layout);
		}

		unsigned int indices[3] = { 0, 1, 2 };
		std::shared_ptr<IndexBuffer> index_buffer;
		index_buffer.reset(IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));

		triangle_va_->addVertexBuffer(vertex_buffer);
		triangle_va_->setIndexBuffer(index_buffer);


		std::string vertex_string = R"(
			#version 450 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				//v_Color = vec4(0.2, 0.8, 0.3, 1.0);
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);	
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

		triangle_shader_.reset(new Shader(vertex_string, fragment_string));







		rectangle_va_.reset(VertexArray::create());
		float rect_vertices[3 * 4] =
		{
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};
		std::shared_ptr<VertexBuffer> rect_vb;
		rect_vb.reset(VertexBuffer::create(rect_vertices, sizeof(rect_vertices)));
		BufferLayout rectangle_layout =
		{
			{ShaderDataType::Float3,"a_Position"}
		};
		rect_vb->setLayout(rectangle_layout);
		rectangle_va_->addVertexBuffer(rect_vb);

		uint32_t rect_indices[] = { 0,1,2,2,3,0 };
		std::shared_ptr<IndexBuffer> rect_ib;
		rect_ib.reset(IndexBuffer::create(rect_indices, sizeof(rect_indices)));
		rectangle_va_->setIndexBuffer(rect_ib);

		std::string rect_v_src = R"(
			#version 450 core
			
			layout(location = 0) in vec3 a_Position;
			out vec3 v_Position;
			void main()
			{
				gl_Position = vec4(a_Position, 1.0);	
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

		rectangle_shader_.reset(new Shader(rect_v_src, rect_f_src));
	
	}

	Application::~Application()
	{

	}

	void Application::initWindow(WindowProps props)
	{

	}

	void Application::run()
	{
		WindowResizeEvent ev(1280, 720);
		DN_CORE_INFO(ev);
		DN_CORE_INFO("{0}, {1}", "app",std::this_thread::get_id());
		while (is_running_)
		{
			RenderCommand::setClearColor({ 0.2, 0.2, 0.2, 1 });
			RenderCommand::clear();

			Renderer::beginScene();

			rectangle_shader_->bind();
			Renderer::submit(rectangle_va_);

			triangle_shader_->bind();
			Renderer::submit(triangle_va_);


			Renderer::endScene();


			for (Layer* layer : layer_stack_)
			{
				layer->onUpdate();
			}

			imgui_layer_->begin();
			for (Layer* layer : layer_stack_)
			{
				layer->onImGuiRender();
			}
			imgui_layer_->end();

			window_->onUpdate();

			triangle_shader_->unBind();
		}

		//DN_CORE_INFO("{0}", "application end");
	}

	void Application::onEvent(Event& ev)
	{
		EventDispatcher dispatcher(ev);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(onWindowClose));

		DN_CORE_TRACE("{0}", ev);

		for (auto it = layer_stack_.end(); it != layer_stack_.begin();)
		{
			(*--it)->onEvent(ev);
			if (ev.isHandled())
			{
				break;
			}
		}
	}

	void Application::pushLayer(Layer* layer)
	{
		layer_stack_.pushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(Layer* layer)
	{
		layer_stack_.pushOverlay(layer);
		layer->onAttach();
	}

	bool Application::onWindowClose(WindowCloseEvent& ev)
	{
		is_running_ = false;
		return true;
	}
}

