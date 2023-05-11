#include "application.h"
#include "engine_log.h"
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

	}
}

