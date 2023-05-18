#include "donut.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
	glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.f);
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return Projection * View * Model;
}

class ExampleLayer : public Donut::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{
		auto cam = camera(5.0f, { 0.5f, 0.5f });
	}

	void onUpdate() override
	{
		//DN_CLIENT_INFO("Example::onUpdate");
		if (Donut::Input::isKeyPressed(DN_KEY_TAB))
		{
			DN_CLIENT_INFO("{0}", "Tab key is pressed!");
		}
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
		pushOverlay(new Donut::ImGuiLayer());
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
