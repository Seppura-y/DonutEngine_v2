#include "application.h"

namespace Donut
{
	Application* Application::s_instance_ = nullptr;

	Application::Application()
	{
		s_instance_ = this;
	}

	Application::~Application()
	{

	}

	void Application::run()
	{

	}
}

