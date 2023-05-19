#pragma once
#include "core/window.h"
#include "core/logger.h"

#include "renderer/graphics_context.h"

struct GLFWwindow;
namespace Donut {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void onUpdate() override;

		inline uint32_t getWidth() const override { return win_data_.width; }
		inline uint32_t getHeight() const override { return win_data_.height; }

		// Window attributes
		inline void setEventCallback(const EventCallbackFn& callback) override { win_data_.event_callback = callback; }
		void setVSync(bool enabled) override;
		bool isVSync() const override;

		inline virtual void* getNativeWindow() const override { return glfw_window_; };
	private:
		virtual void init(const WindowProps& props);
		virtual void shutdown();
	private:
		GLFWwindow* glfw_window_;
		GraphicsContext* graphics_ctx_;
		struct WindowData
		{
			std::string title;
			unsigned int width, height;
			bool is_vsync;

			EventCallbackFn event_callback;
		};

		WindowData win_data_;
	};

}