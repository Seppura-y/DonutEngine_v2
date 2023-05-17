#ifndef ENTRY_POINT_H
#define ENTRY_POINT_H
#include "application.h"
#include <iostream>

#ifdef DN_PLATFORM_WINDOWS

extern Donut::Application* Donut::createApplication();	//Defined at client

int main(int argc, char** argv)
{
	Donut::Logger::init();
	DN_CORE_TRACE("log init");
	DN_CLIENT_TRACE("client init");
	auto app = Donut::createApplication();
	app->run();
	delete app;
}

#endif // DN_PLATFORM_WINDOWS

#endif // ENTRY_POINT_H