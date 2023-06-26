#ifndef ENTRY_POINT_H
#define ENTRY_POINT_H
#include "application.h"

#include <iostream>

#ifdef DN_PLATFORM_WINDOWS

extern Donut::Application* Donut::createApplication(const ApplicationSpecification& spec); //Defined at client

int main(int argc, char** argv)
{
	Donut::Logger::init();
	DN_PROFILE_BEGIN_SESSION("Startup", "DonutProfile-Startup.json");
	Donut::ApplicationSpecification spec;
	spec.commandline_args_ = { argc, argv };
	auto app = Donut::createApplication(spec);
	DN_PROFILE_END_SESSION();

	app->initWindow();

	DN_PROFILE_BEGIN_SESSION("Runtime", "DonutProfile-Runtime.json");
	app->run();
	DN_PROFILE_END_SESSION();

	DN_PROFILE_BEGIN_SESSION("Shutdown", "DonutProfile-Shutdown.json");
	delete app;
	DN_PROFILE_END_SESSION();
}

#endif // DN_PLATFORM_WINDOWS

#endif // ENTRY_POINT_H