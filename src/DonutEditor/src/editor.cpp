#include "DonutEngine/donut.h"

class ExampleLayer : public Donut::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{

	}

	void onUpdate() override
	{
		DN_CLIENT_INFO("Example::onUpdate");
	}

	void onEvent(Donut::Event& ev) override
	{
		DN_CLIENT_TRACE("{0}", ev);
	}
};

class Sandbox : public Donut::Application 
{
public:
	Sandbox()
	{
		pushLayer(new ExampleLayer());
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
