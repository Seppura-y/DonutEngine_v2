#include "pch.h"

#include <glfw/glfw3.h>
#include <glad/glad.h>

#include <chrono>
#include <thread>

#include "core/application.h"
#include "core/logger.h"
#include "core/input.h"

#include "renderer/renderer.h"
#include "renderer/renderer_2d.h"

#include "utils/platform_utils.h"

namespace Donut
{
#define BIND_EVENT_FN(x) std::bind(&Application::x,this,std::placeholders::_1)

	Application* Application::s_instance_ = nullptr;

	static GLenum shaderDataTypeToGLenumType(const ShaderDataType& type)
	{
		switch (type)
		{
			case ShaderDataType::Float:		return GL_FLOAT;
			case ShaderDataType::Float2:	return GL_FLOAT;
			case ShaderDataType::Float3:	return GL_FLOAT;
			case ShaderDataType::Float4:	return GL_FLOAT;
			case ShaderDataType::Int:		return GL_INT;
			case ShaderDataType::Int2:		return GL_INT;
			case ShaderDataType::Int3:		return GL_INT;
			case ShaderDataType::Int4:		return GL_INT;
			case ShaderDataType::Bool:		return GL_BOOL;
		}

		DN_CORE_ASSERT(false, "Unknown shader data type!");
		return 0;
	}

	Application::Application(const ApplicationSpecification& spec)
		: specification_(spec)
	{
		DN_PROFILE_FUNCTION();
		s_instance_ = this;

		if (!specification_.working_dir_.empty())
		{
			std::filesystem::current_path(specification_.working_dir_);
		}
		window_ = std::unique_ptr<Window>(Window::create(WindowProps(specification_.name_)));
		window_->setEventCallback(BIND_EVENT_FN(onEvent));
		//window_->setVSync(false);

		Renderer::init();
		Renderer2D::init();

		imgui_layer_ = new ImGuiLayer();
		pushOverlay(imgui_layer_);

		is_running_ = true;
	}

	Application::~Application()
	{

	}

	void Application::initWindow(WindowProps props)
	{

	}

	void Application::run()
	{
		DN_PROFILE_FUNCTION();

		WindowResizeEvent ev(1600, 900);
		DN_CORE_INFO(ev);
		DN_CORE_INFO("{0}, {1}", "app",std::this_thread::get_id());
		while (is_running_)
		{
			DN_PROFILE_SCOPE("runloop");
			//float time = (float)glfwGetTime();
			float time = Time::getTime();
			Timestep timestep = time - last_frame_time_;
			last_frame_time_ = time;

			// when the window is minimized, then we don't need to render anything in the viewport.
			if (!is_minimized_)
			{
				{
					DN_PROFILE_SCOPE("LayerStack -- onUpdate");

					for (Layer* layer : layer_stack_)
					{
						layer->onUpdate(timestep);
					}
				}
				imgui_layer_->begin();
				{
					DN_PROFILE_SCOPE("LayerStack -- onImGuiRender");

					for (Layer* layer : layer_stack_)
					{
						layer->onImGuiRender();
					}
				}

				imgui_layer_->end();
			}




			window_->onUpdate();
		}

		//DN_CORE_INFO("{0}", "application end");
	}

	void Application::onEvent(Event& ev)
	{
		DN_PROFILE_FUNCTION();

		EventDispatcher dispatcher(ev);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(onWindowResize));

		for (auto it = layer_stack_.end(); it != layer_stack_.begin();)
		{
			if (ev.isHandled())
			{
				break;
			}
			(*--it)->onEvent(ev);
		}
	}

	void Application::pushLayer(Layer* layer)
	{
		layer_stack_.pushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(Layer* layer)
	{
		layer_stack_.pushOverlay(layer);
		layer->onAttach();
	}

	bool Application::onWindowClose(WindowCloseEvent& ev)
	{
		is_running_ = false;
		return true;
	}

	bool Application::onWindowResize(WindowResizeEvent& ev)
	{
		DN_PROFILE_FUNCTION();

		if (ev.getWidth() == 0 || ev.getHeight() == 0)
		{
			is_minimized_ = true;

		}
		else
		{
			is_minimized_ = false;
			Renderer::onWindowResize(ev.getWidth(), ev.getHeight());
		}

		return false;
	}
}

