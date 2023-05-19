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

		glGenBuffers(1, &vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);

		float vertices[3 * 3] =
		{
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		glGenBuffers(1, &ebo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

		unsigned int indices[3] = { 0, 1, 2 };
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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

			glBindVertexArray(vao_);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

			//for (Layer* layer : layer_stack_)
			//{
			//	layer->onUpdate();
			//}

			//imgui_layer_->begin();
			//for (Layer* layer : layer_stack_)
			//{
			//	layer->onImGuiRender();
			//}
			//imgui_layer_->end();

			window_->onUpdate();
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

