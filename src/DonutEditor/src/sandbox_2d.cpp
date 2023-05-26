#include "sandbox_2d.h"
#include "imgui/imgui.h"

#include "platform/opengl/opengl_shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Donut::Layer("sandbox 2d"), camera_controller_(1600.0f / 900.0f, true)
{

}


void Sandbox2D::onAttach()
{

}

void Sandbox2D::onDetach()
{

}

void Sandbox2D::onUpdate(Donut::Timestep ts)
{
	camera_controller_.onUpdate(ts);

	Donut::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Donut::RenderCommand::clear();

	Donut::Renderer2D::beginScene(camera_controller_.getCamera());

	Donut::Renderer2D::drawRectangle(glm::vec2{ 0.5f, 0.8f }, glm::vec2{ 1.2f, 1.0f }, glm::vec4{ 0.8f, 0.2f, 0.3f, 1.0f });
	Donut::Renderer2D::drawRectangle(glm::vec2{ -1.0f, 0.0f }, glm::vec2{ 1.0f, 1.0f }, glm::vec4{0.2f, 0.3f, 0.8f, 1.0f});
	
	Donut::Renderer2D::endScene();
}

void Sandbox2D::onEvent(Donut::Event& ev)
{
	camera_controller_.onEvent(ev);
}

void Sandbox2D::onImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Rectangle Color", glm::value_ptr(rectangle_color_));
	ImGui::End();
}
