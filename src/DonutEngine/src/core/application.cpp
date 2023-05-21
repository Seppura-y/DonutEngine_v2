#include "pch.h"

#include <glfw/glfw3.h>
#include <glad/glad.h>

#include <chrono>
#include <thread>

#include "core/application.h"
#include "core/logger.h"
#include "core/input.h"

#include "renderer/renderer.h"

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

	Application::Application()
	{
		s_instance_ = this;
		window_ = std::unique_ptr<Window>(Window::create(WindowProps()));
		window_->setEventCallback(BIND_EVENT_FN(onEvent));

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
		WindowResizeEvent ev(1280, 720);
		DN_CORE_INFO(ev);
		DN_CORE_INFO("{0}, {1}", "app",std::this_thread::get_id());
		while (is_running_)
		{

			for (Layer* layer : layer_stack_)
			{
				layer->onUpdate();
			}

			imgui_layer_->begin();
			for (Layer* layer : layer_stack_)
			{
				layer->onImGuiRender();
			}
			imgui_layer_->end();

			window_->onUpdate();
		}

		//DN_CORE_INFO("{0}", "application end");
	}

	void Application::onEvent(Event& ev)
	{
		EventDispatcher dispatcher(ev);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(onWindowClose));

		DN_CORE_TRACE("{0}", ev);

		for (auto it = layer_stack_.end(); it != layer_stack_.begin();)
		{
			(*--it)->onEvent(ev);
			if (ev.isHandled())
			{
				break;
			}
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
}

