#include "donut.h"

#include "imgui/imgui.h"
#include "renderer/graphics_context.h"
#include "platform/windows/windows_window.h"
#include "platform/opengl/opengl_context.h"

//#pragma comment(lib, "DonutEngine2.lib")

class ExampleLayer : public Donut::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{

	}

	void onUpdate() override
	{
		DN_CLIENT_INFO("Example::onUpdate");
		if (Donut::Input::isKeyPressed(DN_KEY_TAB))
		{
			DN_CLIENT_INFO("{0}", "Tab key is pressed!");
		}
	}

	virtual void onImGuiRender() override
	{
		ImGui::Begin("test begin");
		ImGui::Text("sdf");
		ImGui::End();
	}

	void onEvent(Donut::Event& ev) override
	{
		DN_CLIENT_TRACE("{0}", ev);
		if (ev.getEventType() == Donut::EventType::KeyPressed)
		{
			Donut::KeyPressedEvent& e = (Donut::KeyPressedEvent&)ev;
			DN_CLIENT_TRACE("{0}", (char)e.getKeyCode());
		}
	}
};

class Sandbox : public Donut::Application
{
public:
	Sandbox()
	{
		pushLayer(new ExampleLayer());
		//pushOverlay(new Donut::ImGuiLayer());
	}

	~Sandbox()
	{

	}

	void windowUpdate() override
	{

	}
};

Donut::Application* Donut::createApplication()
{
	return new Sandbox();
}



#include "core/application.h"

#include <iostream>

#ifdef DN_PLATFORM_WINDOWS

extern Donut::Application* Donut::createApplication();	//Defined at client

int main(int argc, char** argv)
{
	Donut::Logger::init();
	//DN_CORE_TRACE("log init");
	//DN_CLIENT_TRACE("client init");
	auto app = Donut::createApplication();
	app->initWindow();
	app->run();
	delete app;
}

#endif // DN_PLATFORM_WINDOWS

