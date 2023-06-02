#ifndef EDITOR_LAYER_H
#define EDITOR_LAYER_H
#include "donut.h"
#include "renderer/subtexture.h"

namespace Donut
{
	class EditorLayer : public Donut::Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		virtual void onUpdate(Donut::Timestep ts) override;
		virtual void onImGuiRender() override;
		virtual void onEvent(Donut::Event& ev) override;

	private:
		struct ProfileResult
		{
			const char* name_;
			float time_;
		};

		std::vector<ProfileResult> profile_results_;

	private:
		Donut::OrthographicCameraController camera_controller_;

		Donut::Ref<Donut::VertexArray> rectangle_va_;
		Donut::Ref<Donut::Shader> rectangle_shader_;
		Donut::Ref<Donut::Texture2D> rectangle_texture_;
		Donut::Ref<Donut::Texture2D> sprite_texture_;

		Donut::Ref<Donut::Framebuffer> framebuffer_;

		glm::vec4 rectangle_color_{ 0.2f, 0.3f, 0.8f, 1.0f };

		Donut::Ref<Donut::Subtexture> texture_stair_, texture_tree_, texture_barrel_;
		Donut::Ref<Donut::Subtexture> texture_water_, texture_dirt_;

		uint32_t map_width_, map_height_;
		std::unordered_map<char, Donut::Ref<Donut::Subtexture>> tile_map_;

		glm::vec2 viewport_size_{ 0,0 };

		bool is_viewport_focused_ = false;
		bool is_viewport_hovered_ = false;
	};
}
#endif