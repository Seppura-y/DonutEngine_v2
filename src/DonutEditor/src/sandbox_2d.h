#ifndef SANDBOX_2D_H
#define SANDBOX_2D_H

#include "donut.h"

#include "particle_system.h"
#include "renderer/subtexture.h"

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
	Donut::Ref<Donut::Texture2D> sprite_texture_;

	Donut::Ref<Donut::Framebuffer> framebuffer_;

	glm::vec4 rectangle_color_{ 0.2f, 0.3f, 0.8f, 1.0f };

	ParticleSystem particle_system_;
	ParticleProps particle_props_;

	Donut::Ref<Donut::Subtexture> texture_stair_, texture_tree_, texture_barrel_;
	Donut::Ref<Donut::Subtexture> texture_water_, texture_dirt_;

	uint32_t map_width_, map_height_;
	std::unordered_map<char, Donut::Ref<Donut::Subtexture>> tile_map_;
};

#endif