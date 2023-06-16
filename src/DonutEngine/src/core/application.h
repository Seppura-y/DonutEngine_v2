#ifndef APPLICATION_H
#define APPLICATION_H

#include "core/core.h"
#include "core/window.h"
#include "core/layer_stack.h"
#include "core/timestep.h"

#include "events/event.h"
#include "events/application_event.h"

#include "imgui/imgui_layer.h"

namespace Donut
{
	struct ApplicationCommandLineArgs
	{
		int count_ = 0;
		char** args_ = nullptr;

		const char* operator[](int index) const
		{
			DN_CORE_ASSERT(index < count_, "invalid index");
			return args_[index];
		}
	};

	class DONUT_API Application
	{
	public:
		//Application();
		Application(const std::string& name = "DonutEngine", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
		virtual ~Application();

		void run();
		void close() { is_running_ = false; }
		virtual void windowUpdate() = 0;

		void onEvent(Event& ev);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);

		inline static Application& getInstance() { return *s_instance_; }
		inline Window& getWindow() { return *window_; }
		ImGuiLayer* getImGuiLayer() { return imgui_layer_; }

		void initWindow(WindowProps props = WindowProps());

		ApplicationCommandLineArgs getCommandLineArgs() const { return commandline_args_; }
	private:
		bool onWindowClose(WindowCloseEvent& ev);
		bool onWindowResize(WindowResizeEvent& ev);
	private:
		static Application* s_instance_;
		bool is_running_ = false;
		std::unique_ptr<Window> window_;
		ImGuiLayer* imgui_layer_;

		LayerStack layer_stack_;
		float last_frame_time_ = 0.0f;

		bool is_minimized_ = false;

		ApplicationCommandLineArgs commandline_args_;
	};

	// To be defined in client
	Application* createApplication(ApplicationCommandLineArgs args);
}
#endif