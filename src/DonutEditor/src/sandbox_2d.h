#ifndef SANDBOX_2D_H
#define SANDBOX_2D_H

#include "donut.h"

class Sandbox2D : public Donut::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	virtual void onUpdate(Donut::Timestep ts) override;
	virtual void onEvent(Donut::Event& ev) override;
	virtual void onImGuiRender() override;
private:
	Donut::OrthographicCameraController camera_controller_;

	Donut::Ref<Donut::VertexArray> rectangle_va_;
	Donut::Ref<Donut::Shader> rectangle_shader_;
	Donut::Ref<Donut::Texture2D> rectangle_texture_;

	glm::vec4 rectangle_color_{ 0.2f, 0.3f, 0.8f, 1.0f };
};

#endif