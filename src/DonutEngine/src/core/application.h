#ifndef APPLICATION_H
#define APPLICATION_H

#include "core/core.h"
#include "events/event.h"
#include "events/application_event.h"
#include "core/window.h"

namespace Donut
{
	class DONUT_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();
		virtual void windowUpdate() = 0;

		void onEvent(Event& ev);

		inline static Application& getInstance() { return *s_instance_; }

	private:
		bool onWindowClose(WindowCloseEvent& ev);
	private:
		static Application* s_instance_;
		bool is_running_ = false;
		std::unique_ptr<Window> window_;
	};

	// To be defined in client
	Application* createApplication();
}
#endif