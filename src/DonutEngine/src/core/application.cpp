#include "application.h"
#include "logger.h"

#include <glfw/glfw3.h>
#include <glad/glad.h>

#include <chrono>
#include <thread>


namespace Donut
{
#define BIND_EVENT_FN(x) std::bind(&Application::x,this,std::placeholders::_1)

	//Application* Application::s_instance_ = nullptr;

	Application::Application()
	{
		s_instance_ = this;
		is_running_ = true;
	}

	Application::~Application()
	{

	}

	void Application::initWindow(WindowProps props)
	{
		window_ = std::unique_ptr<Window>(Window::create(props));
		window_->setEventCallback(BIND_EVENT_FN(onEvent));
	}

	void Application::run()
	{
		WindowResizeEvent ev(1280, 720);
		DN_CORE_INFO(ev);
		DN_CORE_INFO("{0}, {1}", "app",std::this_thread::get_id());
		while (is_running_)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : layer_stack_)
			{
				layer->onUpdate();
			}
			std::this_thread::sleep_for(std::chrono::microseconds(10));
			windowUpdate();
		}
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
	}

	void Application::pushOverlay(Layer* layer)
	{
		layer_stack_.pushOverlay(layer);
	}

	bool Application::onWindowClose(WindowCloseEvent& ev)
	{
		is_running_ = false;
		return true;
	}
}

