#include "sandbox_2d.h"
#include "imgui/imgui.h"

#include "platform/opengl/opengl_shader.h"
#include "platform/opengl/opengl_texture.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Donut::Layer("sandbox 2d"), camera_controller_(1600.0f / 900.0f, true)
{

}


void Sandbox2D::onAttach()
{
	rectangle_texture_ = Donut::Texture2D::createTexture("assets/textures/checkbox.png");
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

	rectangle_texture_->bind();
	Donut::Renderer2D::drawRectangle(glm::vec3{ 0.5f, 0.8f, 0.1f }, glm::vec2{ 0.5f, 0.5f }, glm::vec4{ 0.8f, 0.2f, 0.3f, 1.0f });
	Donut::Renderer2D::drawRectangle(glm::vec3{ 1.0f, 0.5f, 0.2f }, glm::vec2{ 0.8f, 0.6f }, glm::vec4{ 0.2f, 0.3f, 0.8f, 1.0f });
	Donut::Renderer2D::drawRectangle(glm::vec3{ 0.0f, 0.0f, -0.1f }, glm::vec2{ 10.0f, 10.0f }, rectangle_texture_);
	
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
