#include "sandbox_2d.h"
#include "imgui/imgui.h"

#include "platform/opengl/opengl_shader.h"
#include "platform/opengl/opengl_texture.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

template<typename Fn>
class Timer
{
public:
	Timer(const char* name, Fn&& func)
		: name_(name), is_stopped_(false), func_(func)
	{
		start_time_ = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!is_stopped_)
		{
			stop();
		}
	}

	void stop()
	{
		auto end_time = std::chrono::high_resolution_clock::now();
		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(start_time_).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(end_time).time_since_epoch().count();

		is_stopped_ = true;

		float duration = (end - start) * 0.001f;

		std::cout << "duration : " << duration << "ms" << std::endl;
		func_({ name_, duration });
	}


private:
	const char* name_;
	std::chrono::time_point<std::chrono::steady_clock> start_time_;
	bool is_stopped_;
	Fn func_;
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profile_result) { profile_results_.push_back(profile_result);} )

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
	//Timer timer("Sandbox2D::onUpdate", [&](auto profile_result) { profile_results_.push_back(profile_result)});
	PROFILE_SCOPE("Sandbox2D::onUpdate");

	{
		PROFILE_SCOPE("Camera_controler.onUpdate");
		camera_controller_.onUpdate(ts);
	}

	{
		PROFILE_SCOPE("RenderCommand::clear");
		Donut::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Donut::RenderCommand::clear();
	}


	{
		PROFILE_SCOPE("Renderer2D - draw");

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
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Rectangle Color", glm::value_ptr(rectangle_color_));
	for (auto result : profile_results_)
	{
		char label[50];
		strcpy(label, "%.3f ms  ");
		strcat(label, result.name_);
		ImGui::Text(label, result.time_);
	}
	profile_results_.clear();
	ImGui::End();
}
