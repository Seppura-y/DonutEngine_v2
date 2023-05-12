#include "application.h"
#include "engine_log.h"

#include <chrono>
#include <thread>

namespace Donut
{
#define BIND_EVENT_FN(x) std::bind(&Application::x,this,std::placeholders::_1)

	Application* Application::s_instance_ = nullptr;

	Application::Application()
	{
		s_instance_ = this;
		EngineLog::init();

		window_ = std::unique_ptr<Window>(Window::create());
		window_->setEventCallback(BIND_EVENT_FN(onEvent));
	}

	Application::~Application()
	{

	}

	void Application::run()
	{
		WindowResizeEvent ev(1280, 720);
		DN_ENGINE_INFO(ev);
		DN_ENGINE_INFO("{0}, {1}", "app",std::this_thread::get_id());
		while (is_running_)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(10));
			windowUpdate();
		}
	}

	void Application::onEvent(Event& ev)
	{
		EventDispatcher dispatcher(ev);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(onWindowClose));

	}

	bool Application::onWindowClose(WindowCloseEvent& ev)
	{
		is_running_ = false;
		return true;
	}
}

