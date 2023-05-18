#include "donut.h"
#include "imgui/imgui.h"

class ExampleLayer : public Donut::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{

	}

	void onUpdate() override
	{
		//DN_CLIENT_INFO("Example::onUpdate");
		if (Donut::Input::isKeyPressed(DN_KEY_TAB))
		{
			DN_CLIENT_INFO("{0}", "Tab key is pressed!");
		}
	}

	virtual void onImGuiRender() override
	{
		//ImGui::Begin("test begin");
		//ImGui::Text("sdf");
		//ImGui::End();
	}

	void onEvent(Donut::Event& ev) override
	{
		//DN_CLIENT_TRACE("{0}", ev);
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
