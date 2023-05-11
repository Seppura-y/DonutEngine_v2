#ifndef APPLICATION_H
#define APPLICATION_H

#include "core.h"
#include "events/event.h"

namespace Donut
{
	class DONUT_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();
		virtual void windowUpdate() = 0;

		inline static Application& getInstance() { return *s_instance_; }
	private:
		static Application* s_instance_;

	};

	// To be defined in client
	Application* createApplication();
}
#endif