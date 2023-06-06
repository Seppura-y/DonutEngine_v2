#include "editor_layer.h"

#include "imgui/imgui.h"

#include "platform/opengl/opengl_shader.h"
#include "platform/opengl/opengl_texture.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

static const uint32_t tile_width = 24;
static const char* map_tiles =
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWDDDDDDDDDDWWWWWWW"
"WWWWDDDDDDDDDDDDDDDDWWWW"
"WWWDDDDDWDDDDDDDDDDDDDWW"
"WWDDDDDWWWDDDDDDDDDDWWW"
"WWWDDDDWWDDDDDDDDDDDDWWW"
"WWWWDDDDDDDDDDDDDDDWWWWW"
"WWWWWWDDDDDDDDDDDDWWWWWW"
"WWWWWWWWWWDDDDDDDWWWWWWW"
"WWWWWWWWWWDDDDDWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWCWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW";

Donut::EditorLayer::EditorLayer()
	: Donut::Layer("sandbox 2d"),
	camera_controller_(1600.0f / 900.0f, true),
	rectangle_color_({ 0.2f, 0.3f, 0.8f, 1.0f })
{

}


void Donut::EditorLayer::onAttach()
{
	DN_PROFILE_FUNCTION();

	//rectangle_texture_ = Donut::Texture2D::createTexture("assets/textures/checkbox.png");
	//sprite_texture_ = Donut::Texture2D::createTexture("assets/textures/RPG Base/RPGpack_sheet_2X.png");

	//texture_stair_ = Donut::Subtexture::createFromCoordinate(sprite_texture_, { 7,6 }, { 1,1 }, { 128,128 });
	//texture_tree_ = Donut::Subtexture::createFromCoordinate(sprite_texture_, { 2,1 }, { 1,2 }, { 128,128 });
	//texture_barrel_ = Donut::Subtexture::createFromCoordinate(sprite_texture_, { 8,2 }, { 1,1 }, { 128,128 });

	//texture_water_ = Donut::Subtexture::createFromCoordinate(sprite_texture_, { 1,11 }, { 1,1 }, { 128,128 });
	//texture_dirt_ = Donut::Subtexture::createFromCoordinate(sprite_texture_, { 6,11 }, { 1,1 }, { 128,128 });
	//tile_map_.insert({ 'W', texture_water_ });
	//tile_map_.insert({ 'D', texture_dirt_ });

	//map_width_ = tile_width;
	//map_height_ = strlen(map_tiles) / tile_width;

	//camera_controller_.setZoomLevel(1.0f);

	Donut::FramebufferSpecification framebuffer_spec;
	framebuffer_spec.width_ = 1280;
	framebuffer_spec.height_ = 720;
	framebuffer_ = Donut::Framebuffer::createFramebuffer(framebuffer_spec);


	active_scene_ = createRef<Scene>();
	auto rect = active_scene_->createEntity("Test Rect");
	rect.addComponent<SpriteRendererComponent>(glm::vec4{ 1.0f });

	rect_entity_ = rect;
}

void Donut::EditorLayer::onDetach()
{

}

void Donut::EditorLayer::onUpdate(Donut::Timestep ts)
{
	//Timer timer("Donut::EditorLayer::onUpdate", [&](auto profile_result) { profile_results_.push_back(profile_result)});
	DN_PROFILE_FUNCTION();

	if (is_viewport_focused_)
	{
		camera_controller_.onUpdate(ts);
	}

	Donut::Renderer2D::resetStatistics();

	{
		DN_PROFILE_SCOPE("RenderCommand::clear");
		framebuffer_->bind();
		Donut::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Donut::RenderCommand::clear();
	}

	Renderer2D::beginScene(camera_controller_.getCamera());

	active_scene_->onUpdate(ts);

	Renderer2D::endScene();


	//{
	//	static float rotation = 0.0f;
	//	rotation += ts * 20;

	//	DN_PROFILE_SCOPE("Renderer2D - draw");

	//	Donut::Renderer2D::beginScene(camera_controller_.getCamera());
	//	rectangle_texture_->bind();

	//	Donut::Renderer2D::drawRectangle(glm::vec3{ 0.5f, 0.5f, 0.0f }, glm::vec2{ 0.5f, 0.75f }, glm::vec4{ 0.8f, 0.2f, 0.3f, 1.0f });
	//	Donut::Renderer2D::drawRectangle(glm::vec3{ -1.0f, 0.0f, 0.0f }, glm::vec2{ 0.5f, 0.25f }, rectangle_color_);
	//	Donut::Renderer2D::drawRectangle(glm::vec3{ 0.0f, 0.0f, -0.2f }, glm::vec2{ 20.0f, 20.0f }, rectangle_texture_, 10.0f);
	//	Donut::Renderer2D::endScene();


	//	Donut::Renderer2D::beginScene(camera_controller_.getCamera());
	//	for (float y = -5.0f; y < 5.0f; y += 0.5f)
	//	{
	//		for (float x = -5.0f; x < 5.0f; x += 0.5f)
	//		{
	//			glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
	//			Donut::Renderer2D::drawRectangle(glm::vec2{ x, y }, glm::vec2{ 0.45, 0.45f }, color);
	//		}
	//	}
	//	Donut::Renderer2D::endScene();


	//	//Donut::Renderer2D::beginScene(camera_controller_.getCamera());
	//	//sprite_texture_->bind();
	//	//Donut::Renderer2D::drawRectangle(glm::vec3{ 0.0f, 0.0f, 0.5f }, glm::vec2{ 1.0f, 1.0f }, sprite_texture_, 1.0f);
	//	//Donut::Renderer2D::endScene();
	//}

	if (Donut::Input::isMouseButtonPressed(DN_MOUSE_BUTTON_LEFT))
	{
		auto [x, y] = Donut::Input::getMousePosition();
		auto width = Donut::Application::getInstance().getWindow().getWidth();
		auto height = Donut::Application::getInstance().getWindow().getHeight();

		auto bounds = camera_controller_.getBounds(); // we need to know the bound of our camera so that we can actually draw or emit the particle in the correct place 
		auto pos = camera_controller_.getCamera().getPosition();
		x = (x / width) * bounds.getWidth() - bounds.getWidth() * 0.5f;
		y = bounds.getHeight() * 0.5f - (y / height) * bounds.getHeight();

	}


	//Donut::Renderer2D::beginScene(camera_controller_.getCamera());
	////Donut::Renderer2D::drawRectangle(glm::vec3{ 0.0f, 0.0f, 0.5f }, glm::vec2{ 1.0f, 1.0f }, 0.0f, texture_stair_, 1.0f);

	//for (int y = 0; y < map_height_; y++)
	//{
	//	for (int x = 0; x < map_width_; x++)
	//	{
	//		Donut::Ref<Donut::Subtexture> texture;

	//		char s = map_tiles[x + y * map_width_];
	//		if (tile_map_.find(s) != tile_map_.end())
	//		{
	//			texture = tile_map_[s];
	//		}
	//		else
	//		{
	//			texture = texture_barrel_;
	//		}

	//		Donut::Renderer2D::drawRectangle(glm::vec3{ x - map_width_ / 2.0f, map_height_ / 2.0f - y, 0.5f }, glm::vec2{ 1.0f,1.0f }, 1.0f, texture, 1.0f);
	//	}
	//}
	//Donut::Renderer2D::endScene();

	framebuffer_->unBind();
}

void Donut::EditorLayer::onEvent(Donut::Event& ev)
{
	camera_controller_.onEvent(ev);
}

void Donut::EditorLayer::onImGuiRender()
{
	DN_PROFILE_FUNCTION();

	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & /*ImGuiDockNodeFlags_PassthruDockspace*/ ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows, 
			// which we can't undo at the moment without finer window depth/z control.
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

			if (ImGui::MenuItem("Exit")) Donut::Application::getInstance().close();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::Begin("Settings");

	auto stats = Donut::Renderer2D::getStatistics();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.drawcalls_);
	ImGui::Text("Quads: %d", stats.rect_count_);
	ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
	ImGui::Text("Indices: %d", stats.getTotalIndexCount());

	if (rect_entity_)
	{
		ImGui::Separator();

		auto& tag = rect_entity_.getComponent<TagComponent>();
		ImGui::Text("%s", tag.tag_.c_str());

		auto& rect_color = rect_entity_.getComponent<SpriteRendererComponent>().color;
		ImGui::ColorEdit4("Square Color", glm::value_ptr(rect_color));

		ImGui::Separator();
	}

	ImGui::End();



	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
	ImGui::Begin("Viewport");

	is_viewport_focused_ = ImGui::IsWindowFocused();
	is_viewport_hovered_ = ImGui::IsWindowHovered();
	Application::getInstance().getImGuiLayer()->setBlockEvents(!(is_viewport_focused_ && is_viewport_hovered_));

	ImVec2 viewport_size = ImGui::GetContentRegionAvail();
	if (viewport_size_ != *((glm::vec2*)&viewport_size))
	{
		if (viewport_size.x > 0 || viewport_size.y > 0)
		{
			viewport_size_.x = viewport_size.x;
			viewport_size_.y = viewport_size.y;
			framebuffer_->resize((uint32_t)viewport_size_.x, (uint32_t)viewport_size_.y);

			camera_controller_.onResize(viewport_size_.x, viewport_size_.y);
		}
	}

	uint32_t textureID = framebuffer_->getColorAttachmentID();
	ImGui::Image((void*)textureID, ImVec2{ viewport_size_.x, viewport_size_.y}, ImVec2{0.0f, 1.0f}, ImVec2{1.0f,0.0f});

	ImGui::End();
	ImGui::PopStyleVar();


	ImGui::End();
}
