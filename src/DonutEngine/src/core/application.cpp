#include "application.h"
#include "engine_log.h"

#include "events/application_event.h"

#include <chrono>
#include <thread>

namespace Donut
{
	Application* Application::s_instance_ = nullptr;

	Application::Application()
	{
		s_instance_ = this;
		EngineLog::init();
	}

	Application::~Application()
	{

	}

	void Application::run()
	{
		WindowResizeEvent ev(1280, 720);
		DN_ENGINE_INFO(ev);
		DN_ENGINE_INFO("{0}, {1}", "app",std::this_thread::get_id());
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(10));
			windowUpdate();
		}
	}
}

