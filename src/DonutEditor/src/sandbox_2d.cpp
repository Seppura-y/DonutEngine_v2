#include "sandbox_2d.h"
#include "imgui/imgui.h"

#include "platform/opengl/opengl_shader.h"
#include "platform/opengl/opengl_texture.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

Sandbox2D::Sandbox2D()
	: Donut::Layer("sandbox 2d"), camera_controller_(1600.0f / 900.0f, true)
{

}


void Sandbox2D::onAttach()
{
	DN_PROFILE_FUNCTION();

	rectangle_texture_ = Donut::Texture2D::createTexture("assets/textures/checkbox.png");
}

void Sandbox2D::onDetach()
{

}

void Sandbox2D::onUpdate(Donut::Timestep ts)
{
	//Timer timer("Sandbox2D::onUpdate", [&](auto profile_result) { profile_results_.push_back(profile_result)});
	DN_PROFILE_FUNCTION();

	camera_controller_.onUpdate(ts);

	{
		DN_PROFILE_SCOPE("RenderCommand::clear");
		Donut::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Donut::RenderCommand::clear();
	}


	{
		DN_PROFILE_SCOPE("Renderer2D - draw");

		Donut::Renderer2D::beginScene(camera_controller_.getCamera());

		rectangle_texture_->bind();
		Donut::Renderer2D::drawRectangle(glm::vec3{ 0.5f, 0.8f, 0.1f }, glm::vec2{ 0.5f, 0.5f }, glm::vec4{ 0.8f, 0.2f, 0.3f, 1.0f });
		Donut::Renderer2D::drawRectangle(glm::vec3{ 1.0f, 0.5f, 0.2f }, glm::vec2{ 0.8f, 0.6f }, glm::vec4{ 0.2f, 0.3f, 0.8f, 1.0f });
		Donut::Renderer2D::drawRectangle(glm::vec3{ 0.0f, 0.0f, -0.1f }, glm::vec2{ 10.0f, 10.0f }, rectangle_texture_);

		Donut::Renderer2D::endScene();
	}

}

void Sandbox2D::onEvent(Donut::Event& ev)
{
	camera_controller_.onEvent(ev);
}

void Sandbox2D::onImGuiRender()
{
	DN_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Rectangle Color", glm::value_ptr(rectangle_color_));
	//for (auto result : profile_results_)
	//{
	//	char label[50];
	//	strcpy(label, "%.3f ms  ");
	//	strcat(label, result.name_);
	//	ImGui::Text(label, result.time_);
	//}
	//profile_results_.clear();
	ImGui::End();
}
