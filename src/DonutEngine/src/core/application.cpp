#include "pch.h"
#include "core/application.h"
#include "core/logger.h"

#include "core/input.h"
#include <glfw/glfw3.h>
#include <glad/glad.h>

#include <chrono>
#include <thread>


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

		glGenVertexArrays(1, &vao_);
		glBindVertexArray(vao_);

		float vertices[3 * 7] =
		{
			-0.5f, -0.5f, 0.0f, 0.8f, 0.5f, 0.3f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.8f, 0.5f, 0.3f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.5f, 0.3f, 1.0f
		};

		vertex_buffer_.reset(VertexBuffer::create(vertices, sizeof(vertices)));
		{
			BufferLayout layout = {
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float4, "a_Color" }
			};
			vertex_buffer_->setLayout(layout);
		}
		uint32_t index = 0;
		const auto& layout = vertex_buffer_->getLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.getComponentCount(),
				shaderDataTypeToGLenumType(element.type_),
				element.normalized_ ? GL_TRUE : GL_FALSE,
				layout.getStride(),
				(const void *)element.offset_
			);
			index++;
		}

		unsigned int indices[3] = { 0, 1, 2 };
		index_buffer_.reset(IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));

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

		shader_.reset(new Shader(vertex_string, fragment_string));
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
			glClearColor(0.2, 0.2, 0.2, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			shader_->bind();

			glBindVertexArray(vao_);
			glDrawElements(GL_TRIANGLES, index_buffer_->getIndicesCount() , GL_UNSIGNED_INT, nullptr);

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

			shader_->unBind();
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

