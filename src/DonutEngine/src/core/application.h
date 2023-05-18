#ifndef APPLICATION_H
#define APPLICATION_H

#include "core/core.h"
#include "core/window.h"
#include "core/layer_stack.h"

#include "events/event.h"
#include "events/application_event.h"


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

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);

		inline static Application& getInstance() { return *s_instance_; }
		inline Window& getWindow() { return *window_; }

		void initWindow(WindowProps props = WindowProps());
	private:
		bool onWindowClose(WindowCloseEvent& ev);
	private:
		static Application* s_instance_;
		bool is_running_ = false;
		std::unique_ptr<Window> window_;

		LayerStack layer_stack_;
	};

	// To be defined in client
	Application* createApplication();
}
#endif