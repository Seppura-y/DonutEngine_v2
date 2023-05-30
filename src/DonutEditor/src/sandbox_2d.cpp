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

	Donut::Renderer2D::resetStatistics();

	{
		DN_PROFILE_SCOPE("RenderCommand::clear");
		Donut::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Donut::RenderCommand::clear();
	}


	{
		static float rotation = 0.0f;
		rotation += ts * 20;

		DN_PROFILE_SCOPE("Renderer2D - draw");

		Donut::Renderer2D::beginScene(camera_controller_.getCamera());

		rectangle_texture_->bind();
		//Donut::Renderer2D::drawRectangle(glm::vec3{ 0.5f, 0.8f, 0.1f }, glm::vec2{ 0.5f, 0.5f }, glm::vec4{ 0.8f, 0.2f, 0.3f, 1.0f });
		//Donut::Renderer2D::drawRotatedRectangle(glm::vec3{ 1.0f, 0.5f, 0.2f }, glm::vec2{ 0.8f, 0.6f }, 45.0f, glm::vec4{ 0.2f, 0.3f, 0.8f, 1.0f });
		//Donut::Renderer2D::drawRectangle(glm::vec3{ 0.0f, 0.0f, -0.1f }, glm::vec2{ 10.0f, 10.0f }, rectangle_texture_, 10.0f);
		//Donut::Renderer2D::drawRotatedRectangle(glm::vec3{ 0.0f, 0.0f, -0.1f }, glm::vec2{ 10.0f, 10.0f }, 34.0f, rectangle_texture_, 10.0f, {1.0f, 0.8f, 0.8f, 0.5f});

		Donut::Renderer2D::drawRectangle(glm::vec3{  0.5f, 0.5f, 0.0f }, glm::vec2{ 0.5f, 0.75f }, glm::vec4{ 0.8f, 0.2f, 0.3f, 1.0f });
		Donut::Renderer2D::drawRectangle(glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec2{ 0.5f, 0.25f }, glm::vec4{ 0.2f, 0.2f, 0.8f, 1.0f });
		Donut::Renderer2D::drawRectangle(glm::vec3{  0.0f, 0.0f, -0.2f }, glm::vec2{ 20.0f, 20.0f }, rectangle_texture_, 10.0f);
		//Donut::Renderer2D::drawRectangle(glm::vec3{ -1.3f, -0.8f, 0.0f }, glm::vec2{ 1.0f, 1.0f }, rectangle_texture_, 10.0f);
		//Donut::Renderer2D::drawRotatedRectangle(glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec2{ 1.0f, 1.0f }, rotation, rectangle_texture_, 10.0f);
		Donut::Renderer2D::endScene();
	}

	Donut::Renderer2D::beginScene(camera_controller_.getCamera());
	int c = 0;
	for (float y = -5.0f; y < 5.0f; y += 0.5f)
	{
		for (float x = -5.0f; x < 5.0f; x += 0.5f)
		{
			glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
			Donut::Renderer2D::drawRectangle(glm::vec2{ x, y }, glm::vec2{ 0.45f, 0.45f }, color);
			c++;
		}
	}
	c;
	Donut::Renderer2D::endScene();
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

	auto stats = Donut::Renderer2D::getStatistics();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.drawcalls_);
	ImGui::Text("Rects: %d", stats.rect_count_);
	ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
	ImGui::Text("Indices: %d", stats.getTotalIndexCount());
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
