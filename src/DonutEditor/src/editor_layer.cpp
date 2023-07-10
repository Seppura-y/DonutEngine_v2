#include "editor_layer.h"

#include "imgui/imgui.h"

#include "platform/opengl/opengl_shader.h"
#include "platform/opengl/opengl_texture.h"

#include "scene/scriptable_entity.h"
#include "scene/scene_serializer.h"

#include "core/key_codes.h"

#include "utils/platform_utils.h"

#include "scripting/script_engine.h"

#include "ImGuizmo.h"
#include "math/math.h"


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

extern const std::filesystem::path g_asset_path = "assets";

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

	play_icon_ = Texture2D::createTexture("assets/icons/PlayButton.png");
	stop_icon_ = Texture2D::createTexture("assets/icons/StopButton.png");
	simulate_icon_ = Texture2D::createTexture("assets/icons/SimulateButton.png");
	pause_icon_ = Texture2D::createTexture("assets/icons/PauseButton.png");
	step_icon_ = Texture2D::createTexture("assets/icons/StepButton.png");
	Donut::FramebufferSpecification framebuffer_spec;
	framebuffer_spec.attachments_specifications_ = 
	{
		FramebufferTextureFormat::RGBA8,
		FramebufferTextureFormat::RED_INTEGER,
		FramebufferTextureFormat::Depth 
	};
	framebuffer_spec.width_ = 1280;
	framebuffer_spec.height_ = 720;
	framebuffer_ = Donut::Framebuffer::createFramebuffer(framebuffer_spec);

	editor_scene_ = createRef<Scene>();
	active_scene_ = editor_scene_;
	//auto rect = active_scene_->createEntity("Test Rect");
	//rect.addComponent<SpriteRendererComponent>(glm::vec4{ 1.0f });

	//rect_entity_ = rect;

	//auto rect2 = active_scene_->createEntity("Rect 2");
	//rect2.addComponent<SpriteRendererComponent>(glm::vec4{ 1.0f , 0.0f, 0.0f, 1.0f });

	//auto& trans = rect2.getComponent<TransformComponent>().getTransform();
	//trans = glm::translate(trans, glm::vec3{ 1.0f, 2.0f, 0.0f });

	//fst_camera_ = active_scene_->createEntity("First Camera");
	//fst_camera_.addComponent<CameraComponent>();

	//sec_camera_ = active_scene_->createEntity("Second Camera");
	//auto& camera_component = sec_camera_.addComponent<CameraComponent>();
	//camera_component.is_primary_ = false;


	//class CameraController : public ScriptableEntity
	//{
	//public:
	//	void onCreate()
	//	{
	//		auto& translation = getComponent<TransformComponent>().translation_;
	//		translation.x = rand() % 10 - 5.0f;
	//	}

	//	void onDestroy()
	//	{

	//	}

	//	void onUpdate(Timestep ts)
	//	{
	//		auto& translation = getComponent<TransformComponent>().translation_;
	//		float speed = 5.0f;
	//		
	//		if (Input::isKeyPressed(DN_KEY_A))
	//		{
	//			translation.x -= speed * ts;
	//		}
	//		if (Input::isKeyPressed(DN_KEY_D))
	//		{
	//			translation.x += speed * ts;
	//		}
	//		if (Input::isKeyPressed(DN_KEY_W))
	//		{
	//			translation.y += speed * ts;
	//		}
	//		if (Input::isKeyPressed(DN_KEY_S))
	//		{
	//			translation.y -= speed * ts;
	//		}
	//	}

	//private:

	//};

	//fst_camera_.addComponent<NativeScriptComponent>().bind<CameraController>();
	//sec_camera_.addComponent<NativeScriptComponent>().bind<CameraController>();


	scene_hierarchy_panel_.setContext(active_scene_);
	//SceneSerializer serializer(active_scene_);
	//serializer.deserialize("assets/scenes/example.yaml");

	auto spec = Application::getInstance().getSpecification();
	if (spec.commandline_args_.count_ > 1)
	{
		auto scene_filepath = spec.commandline_args_[1];
		//SceneSerializer serializer(active_scene_);
		//serializer.deserialize(scene_filepath);
		openScene(scene_filepath);
	}

	editor_camera_ = EditorCamera(30.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

	Renderer2D::setLineWidth(4.0f);
}

void Donut::EditorLayer::onDetach()
{

}

void Donut::EditorLayer::onUpdate(Donut::Timestep ts)
{
	//Timer timer("Donut::EditorLayer::onUpdate", [&](auto profile_result) { profile_results_.push_back(profile_result)});
	DN_PROFILE_FUNCTION();

	active_scene_->onViewportResize((uint32_t)viewport_size_.x, (uint32_t)viewport_size_.y);

	// Resize
	if (FramebufferSpecification spec = framebuffer_->getSpecification();
		viewport_size_.x > 0.0f && viewport_size_.y > 0.0f && // zero sized framebuffer is invalid
		(spec.width_ != viewport_size_.x || spec.height_ != viewport_size_.y))
	{
		framebuffer_->resize((uint32_t)viewport_size_.x, (uint32_t)viewport_size_.y);
		camera_controller_.onResize(viewport_size_.x, viewport_size_.y);

		//active_scene_->onViewportResize(viewport_size_.x, viewport_size_.y);

		editor_camera_.setViewportSize(viewport_size_.x, viewport_size_.y);
	}

	//framebuffer_->resize((uint32_t)viewport_size_.x, (uint32_t)viewport_size_.y);
	//camera_controller_.onResize(viewport_size_.x, viewport_size_.y);

	//if (is_viewport_focused_)
	//{
	//	camera_controller_.onUpdate(ts);
	//}
	//editor_camera_.onUpdate(ts);

	Donut::Renderer2D::resetStatistics();

	{
		DN_PROFILE_SCOPE("RenderCommand::clear");
		framebuffer_->bind();
		//Donut::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Donut::RenderCommand::setClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
		Donut::RenderCommand::clear();
	}

	// clear framebuffer value to -1, which is the default entity id for blank area
	framebuffer_->clearAttachment(1, -1);

	switch (scene_state_)
	{
		case SceneState::Edit:
		{
			if (is_viewport_focused_)
			{
				camera_controller_.onUpdate(ts);
			}

			editor_camera_.onUpdate(ts);

			active_scene_->onUpdateEditor(ts, editor_camera_);
			break;
		}
		case SceneState::Play:
		{
			active_scene_->onUpdateRuntime(ts);
			break;
		}
		case SceneState::Simulate:
		{
			editor_camera_.onUpdate(ts);
			active_scene_->onUpdateSimulation(ts, editor_camera_);
			break;
		}
	}
	//active_scene_->onUpdateEditor(ts,editor_camera_);

	auto [x, y] = ImGui::GetMousePos();
	x -= viewport_bounds_[0].x;
	y -= viewport_bounds_[0].y;
	glm::vec2 viewport_size = viewport_bounds_[1] - viewport_bounds_[0];
	y = viewport_size.y - y;

	int mouse_x = (int)x;
	int mouse_y = (int)y;

	if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < (int)viewport_size_.x && mouse_y < (int)viewport_size_.y)
	{
		int pixel_data = framebuffer_->readPixel(1, mouse_x, mouse_y);

		hovered_entity_ = pixel_data == -1 ? Entity() : Entity((entt::entity)pixel_data, active_scene_.get());
		//DN_CORE_WARN("Pixel data = {0}", pixel_data);
	}


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

	onOverlayRender();

	framebuffer_->unBind();
}

void Donut::EditorLayer::onEvent(Donut::Event& ev)
{
	camera_controller_.onEvent(ev);
	if (scene_state_ == SceneState::Edit)
	{
		editor_camera_.onEvent(ev);
	}

	EventDispatcher dispatcher(ev);
	dispatcher.dispatch<KeyPressedEvent>(DN_BIND_EVENT_FN(EditorLayer::onKeyPressed));
	dispatcher.dispatch<MouseButtonPressedEvent>(DN_BIND_EVENT_FN(EditorLayer::onMouseButtonPressed));
}

bool Donut::EditorLayer::onKeyPressed(KeyPressedEvent& ev)
{
	// shortcuts
	if (ev.isRepeat() > 0)
	{
		return false;
	}
	switch (ev.getKeyCode())
	{
	case DN_KEY_S:
		if (Input::isKeyPressed(DN_KEY_LEFT_CONTROL) || Input::isKeyPressed(DN_KEY_RIGHT_CONTROL))
		{
			if (Input::isKeyPressed(DN_KEY_LEFT_ALT) || Input::isKeyPressed(DN_KEY_RIGHT_ALT))
			{
				saveSceneAs();
				break;
			}
			saveScene();
		}
		break;
	case DN_KEY_D:
		if (Input::isKeyPressed(DN_KEY_LEFT_CONTROL) || Input::isKeyPressed(DN_KEY_RIGHT_CONTROL))
		{
			onDuplicateEntity();
		}
		break;
	case DN_KEY_N:
		if (Input::isKeyPressed(DN_KEY_LEFT_CONTROL) || Input::isKeyPressed(DN_KEY_RIGHT_CONTROL))
		{
			newScene();
		}
		break;
	case DN_KEY_O:
		if (Input::isKeyPressed(DN_KEY_LEFT_CONTROL) || Input::isKeyPressed(DN_KEY_RIGHT_CONTROL))
		{
			openScene();
		}
		break;

	case DN_KEY_U:
		if (Input::isKeyPressed(DN_KEY_LEFT_CONTROL) || Input::isKeyPressed(DN_KEY_RIGHT_CONTROL))
		{
			gizmo_type_ = -1;
		}
		break;

	case DN_KEY_I:
		if (Input::isKeyPressed(DN_KEY_LEFT_CONTROL) || Input::isKeyPressed(DN_KEY_RIGHT_CONTROL))
		{
			gizmo_type_ = ImGuizmo::OPERATION::TRANSLATE;
		}
		break;

	case DN_KEY_J:
		if (Input::isKeyPressed(DN_KEY_LEFT_CONTROL) || Input::isKeyPressed(DN_KEY_RIGHT_CONTROL))
		{
			gizmo_type_ = ImGuizmo::OPERATION::ROTATE;
		}
		break;

	case DN_KEY_K:
		if (Input::isKeyPressed(DN_KEY_LEFT_CONTROL) || Input::isKeyPressed(DN_KEY_RIGHT_CONTROL))
		{
			gizmo_type_ = ImGuizmo::OPERATION::SCALE;
		}
		break;

	case DN_KEY_R:
		if (Input::isKeyPressed(DN_KEY_LEFT_CONTROL) || Input::isKeyPressed(DN_KEY_RIGHT_CONTROL))
		{
			ScriptEngine::reloadAssembly();
		}
		else
		{
			if (!ImGuizmo::IsUsing())
			{
				gizmo_type_ = ImGuizmo::OPERATION::SCALE;
			}
		}
		break;

	}
	return false;
}

bool Donut::EditorLayer::onMouseButtonPressed(MouseButtonPressedEvent& ev)
{
	if (ev.getMouseButton() == Mouse::ButtonLeft)
	{
		if (is_viewport_hovered_
			&& !ImGuizmo::IsOver()	// gizmo 被entity遮挡时，不会因点击而丢失选中状态
			&& !Input::isKeyPressed(Key::LeftAlt))	// 按alt调整镜头时，不会丢失选中目标
		{
			scene_hierarchy_panel_.setSelectedEntity(hovered_entity_);
		}
	}
	return false;
}

void Donut::EditorLayer::newScene()
{
	active_scene_ = createRef<Scene>();
	//active_scene_->onViewportResize((uint32_t)viewport_size_.x, (uint32_t)viewport_size_.y);
	scene_hierarchy_panel_.setContext(active_scene_);
	editor_scene_path_ = std::filesystem::path();
}

void Donut::EditorLayer::openScene()
{
	if (scene_state_ != SceneState::Edit)
	{
		onSceneStop();
	}

	std::string filepath = FileDialogs::openFile("Donut Scene (*.yaml)\0*.yaml\0");

	if (!filepath.empty())
	{
		openScene(filepath);
	}
}

void Donut::EditorLayer::openScene(const std::filesystem::path& path)
{
	if (path.extension().string() != ".yaml")
	{
		DN_CORE_WARN("Could not load {0} - not a scene file", path.filename().string());
		return;
	}

	Ref<Scene> new_scene = createRef<Scene>();
	SceneSerializer serializer(new_scene);
	if (serializer.deserialize(path.string()))
	{
		editor_scene_ = new_scene;
		//editor_scene_->onViewportResize((uint32_t)viewport_size_.x, (uint32_t)viewport_size_.y);
		scene_hierarchy_panel_.setContext(editor_scene_);

		active_scene_ = editor_scene_;
		editor_scene_path_ = path;
	}
}

void Donut::EditorLayer::saveScene()
{
	if (!editor_scene_path_.empty())
	{
		serializeScene(active_scene_, editor_scene_path_);
	}
	else
	{
		saveSceneAs();
	}
}

void Donut::EditorLayer::saveSceneAs()
{
	std::string filepath = FileDialogs::openFile("Donut Scene (*.yaml)\0*.yaml\0");
	if (!filepath.empty())
	{
		serializeScene(active_scene_, filepath);
		editor_scene_path_ = filepath;
	}
}

void Donut::EditorLayer::serializeScene(Ref<Scene> scene, const std::filesystem::path& path)
{
	SceneSerializer serializer(scene);
	serializer.serialize(path.string());
}

void Donut::EditorLayer::onSceneStop()
{
	DN_CORE_ASSERT(scene_state_ == SceneState::Play || scene_state_ == SceneState::Simulate);

	if (scene_state_ == SceneState::Play)
	{
		active_scene_->onRuntimeStop();
	}
	else if (scene_state_ == SceneState::Simulate)
	{
		active_scene_->onSimulationStop();
	}

	scene_state_ = SceneState::Edit;

	active_scene_ = editor_scene_;
	scene_hierarchy_panel_.setContext(active_scene_);
}

void Donut::EditorLayer::onScenePlay()
{
	if (scene_state_ == SceneState::Simulate)
	{
		onSceneStop();
	}

	scene_state_ = SceneState::Play;
	active_scene_ = Scene::copyScene(editor_scene_);

	active_scene_->onRuntimeStart();
	scene_hierarchy_panel_.setContext(active_scene_);
}

void Donut::EditorLayer::onSceneSimulate()
{
	if (scene_state_ == SceneState::Play)
	{
		onSceneStop();
	}

	scene_state_ = SceneState::Simulate;

	active_scene_ = Scene::copyScene(editor_scene_);
	active_scene_->onSimulationStart();

	scene_hierarchy_panel_.setContext(active_scene_);
}

void Donut::EditorLayer::onScenePause()
{
	if (scene_state_ == SceneState::Edit)
	{
		return;
	}
	active_scene_->setPaused(true);
}

void Donut::EditorLayer::onDuplicateEntity()
{
	if (scene_state_ != SceneState::Edit)
	{
		return;
	}

	Entity selected_entity = scene_hierarchy_panel_.getSelectedEntity();
	if (selected_entity)
	{
		editor_scene_->duplicateEntity(selected_entity);
	}
}

void Donut::EditorLayer::uiToolbar()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	auto& colors = ImGui::GetStyle().Colors;
	const auto& hovered_color = colors[ImGuiCol_ButtonHovered];
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(hovered_color.x, hovered_color.y, hovered_color.z, 0.5f));
	const auto& active_color = colors[ImGuiCol_ButtonActive];
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(active_color.x, active_color.y, active_color.z, 0.5f));

	ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	bool toolbar_enabled = (bool)active_scene_;

	ImVec4 tint_color = ImVec4(1, 1, 1, 1);
	if (!toolbar_enabled)
	{
		tint_color.w = 0.5f;
	}

	float size = ImGui::GetWindowHeight() - 4.0f;
	ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

	bool has_play_button = scene_state_ == SceneState::Edit || scene_state_ == SceneState::Play;
	bool has_simulate_button = scene_state_ == SceneState::Edit || scene_state_ == SceneState::Simulate;
	bool has_pause_button = scene_state_ != SceneState::Edit;
	if(has_play_button)
	{
		Ref<Texture2D> icon = (scene_state_ == SceneState::Edit || scene_state_ == SceneState::Simulate) ? play_icon_ : stop_icon_;
		if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->getObjectId(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tint_color) && toolbar_enabled)
		{
			if (scene_state_ == SceneState::Edit || scene_state_ == SceneState::Simulate)
				onScenePlay();
			else if (scene_state_ == SceneState::Play)
				onSceneStop();
		}
	}

	//ImGui::SameLine();
	if(has_simulate_button)
	{
		if (has_play_button)
		{
			ImGui::SameLine();
		}

		Ref<Texture2D> icon = (scene_state_ == SceneState::Edit || scene_state_ == SceneState::Play) ? simulate_icon_ : stop_icon_;
		//ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->getObjectId(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tint_color) && toolbar_enabled)
		{
			if (scene_state_ == SceneState::Edit || scene_state_ == SceneState::Play)
				onSceneSimulate();
			else if (scene_state_ == SceneState::Simulate)
				onSceneStop();
		}
	}

	if (has_pause_button)
	{
		bool is_paused = active_scene_->isPaused();
		ImGui::SameLine();
		{
			Ref<Texture2D> icon = pause_icon_;
			if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->getObjectId(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tint_color) && toolbar_enabled)
			{
				active_scene_->setPaused(!is_paused);
			}
		}

		// step button
		if (is_paused)
		{
			ImGui::SameLine();
			{
				Ref<Texture2D> icon = step_icon_;
				bool is_paused = active_scene_->isPaused();
				if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->getObjectId(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tint_color) && toolbar_enabled)
				{
					active_scene_->step();
				}
			}
		}
	}


	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(3);
	ImGui::End();
}

void Donut::EditorLayer::onOverlayRender()
{
	if (scene_state_ == SceneState::Play)
	{
		Entity camera = active_scene_->getPrimaryCameraEntity();
		if (!camera)
		{
			return;
		}
		Renderer2D::beginScene(camera.getComponent<CameraComponent>().camera_, camera.getComponent<TransformComponent>().getTransform());
	}
	else
	{
		Renderer2D::beginScene(editor_camera_);
	}

	if (show_physics_collider_)
	{
		// Box Colliders
		{
			auto view = active_scene_->getAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
			for (auto entity : view)
			{
				auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

				glm::vec3 translation = tc.translation_ + glm::vec3(bc2d.offset_, 0.001f);
				glm::vec3 scale = tc.scale_ * glm::vec3(bc2d.size_ * 2.0f, 1.0f);

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
					* glm::rotate(glm::mat4(1.0f), tc.rotation_.z, glm::vec3(0.0f, 0.0f, 1.0f))
					* glm::scale(glm::mat4(1.0f), scale);

				Renderer2D::drawRectangleWithLines(transform, glm::vec4(0, 1, 0, 1));
			}
		}

		// Circle Colliders
		{
			auto view = active_scene_->getAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
			for (auto entity : view)
			{
				auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

				glm::vec3 translation = tc.translation_ + glm::vec3(cc2d.offset_, 0.001f);
				glm::vec3 scale = tc.scale_ * glm::vec3(cc2d.radius_ * 2.0f);

				glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
					* glm::scale(glm::mat4(1.0f), scale);

				Renderer2D::drawCircle(transform, glm::vec4(0, 1, 0, 1), 0.01f);
			}
		}
	}

	// Selected entity outline
	if (Entity selected_entity = scene_hierarchy_panel_.getSelectedEntity())
	{
		const TransformComponent& transform = selected_entity.getComponent<TransformComponent>();

		Renderer2D::drawRectangleWithLines(transform.getTransform(), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
	}

	Renderer2D::endScene();
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
	ImGuiStyle& style = ImGui::GetStyle();
	float min_win_size = style.WindowMinSize.x;
	style.WindowMinSize.x = 370.0f;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}
	style.WindowMinSize.x = min_win_size;

	if (ImGui::BeginMenuBar())
	{

		if (ImGui::BeginMenu("File"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows, 
			// which we can't undo at the moment without finer window depth/z control.
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

			if (ImGui::MenuItem("New","Ctrl+N"))
			{
				active_scene_ = createRef<Scene>();
				active_scene_->onViewportResize((uint32_t)viewport_size_.x, (uint32_t)viewport_size_.y);
				scene_hierarchy_panel_.setContext(active_scene_);
			}

			if (ImGui::MenuItem("Open...", "Ctrl+O"))
			{
				std::string filepath = FileDialogs::openFile("Donut Scene (*.yaml)\0*.yaml\0");

				if (!filepath.empty())
				{
					active_scene_ = createRef<Scene>();
					active_scene_->onViewportResize((uint32_t)viewport_size_.x, (uint32_t)viewport_size_.y);
					scene_hierarchy_panel_.setContext(active_scene_);

					SceneSerializer serializer(active_scene_);
					serializer.deserialize(filepath);
				}
			}

			if (ImGui::MenuItem("Save As...", "Ctrl+S"))
			{
				std::string filepath = FileDialogs::openFile("Donut Scene (*.yaml)\0*.yaml\0");
				if (!filepath.empty())
				{
					SceneSerializer serializer(active_scene_);
					serializer.serialize(filepath);
				}
			}

			if (ImGui::MenuItem("Exit"))
			{
				Donut::Application::getInstance().close();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Scripts"))
		{
			if (ImGui::MenuItem("Reload assembly", "Ctrl+R"))
			{
				ScriptEngine::reloadAssembly();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	scene_hierarchy_panel_.onImGuiRender();
	content_browser_panel_.onImGuiRender();

	ImGui::Begin("Stats");

	std::string name = "None";
	uint64_t id = 0;
	if (hovered_entity_)
	{
		name = hovered_entity_.getComponent<TagComponent>().tag_;
		id = hovered_entity_.getComponent<IDComponent>().id_;
	}

	ImGui::Text("Hovered Entity : %s : %d", name.c_str(), id);

	auto stats = Donut::Renderer2D::getStatistics();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.drawcalls_);
	ImGui::Text("Quads: %d", stats.rect_count_);
	ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
	ImGui::Text("Indices: %d", stats.getTotalIndexCount());

	//if (rect_entity_)
	//{
	//	ImGui::Separator();

	//	auto& tag = rect_entity_.getComponent<TagComponent>();
	//	ImGui::Text("%s", tag.tag_.c_str());

	//	auto& rect_color = rect_entity_.getComponent<SpriteRendererComponent>().color;
	//	ImGui::ColorEdit4("Square Color", glm::value_ptr(rect_color));

	//	ImGui::Separator();
	//}

	//ImGui::DragFloat3("Camera Transform", glm::value_ptr(fst_camera_.getComponent<TransformComponent>().transform_[3]));

	//if (ImGui::Checkbox("Use First Camera", &is_first_cam_))
	//{
	//	fst_camera_.getComponent<CameraComponent>().is_primary_ = is_first_cam_;
	//	sec_camera_.getComponent<CameraComponent>().is_primary_ = !is_first_cam_;
	//}

	//{
	//	auto& camera = sec_camera_.getComponent<CameraComponent>().camera_;
	//	float ortho_size = camera.getOrthographicSize();
	//	if (ImGui::DragFloat("Second Camera Orthographic Size", &ortho_size))
	//	{
	//		camera.setOrthographicSize(ortho_size);
	//	}
	//}

	ImGui::End();

	ImGui::Begin("settings");
	ImGui::Checkbox("Show physics colliders", &show_physics_collider_);
	ImGui::End();


	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
	ImGui::Begin("Viewport");

	// get viewport position include tab bar
	auto viewport_min_region = ImGui::GetWindowContentRegionMin();
	auto viewport_max_region = ImGui::GetWindowContentRegionMax();
	
	auto viewport_offset = ImGui::GetWindowPos();

	viewport_bounds_[0] = { viewport_min_region.x + viewport_offset.x, viewport_min_region.y + viewport_offset.y };
	viewport_bounds_[1] = { viewport_max_region.x + viewport_offset.x, viewport_max_region.y + viewport_offset.y };

	is_viewport_focused_ = ImGui::IsWindowFocused();
	is_viewport_hovered_ = ImGui::IsWindowHovered();
	Application::getInstance().getImGuiLayer()->setBlockEvents(!is_viewport_hovered_);
	//Application::getInstance().getImGuiLayer()->setBlockEvents(!(is_viewport_focused_ && is_viewport_hovered_));

	ImVec2 viewport_size = ImGui::GetContentRegionAvail();
	if (viewport_size_ != *((glm::vec2*)&viewport_size))
	{
		if (viewport_size.x > 0 || viewport_size.y > 0)
		{
			viewport_size_.x = viewport_size.x;
			viewport_size_.y = viewport_size.y;
			//framebuffer_->resize((uint32_t)viewport_size_.x, (uint32_t)viewport_size_.y);

			//camera_controller_.onResize(viewport_size_.x, viewport_size_.y);
		}
	}

	uint32_t textureID = framebuffer_->getColorAttachmentID();
	ImGui::Image((void*)textureID, ImVec2{ viewport_size_.x, viewport_size_.y}, ImVec2{0.0f, 1.0f}, ImVec2{1.0f,0.0f});

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			openScene(std::filesystem::path(g_asset_path) / path);
		}
		ImGui::EndDragDropTarget();
	}

	// Gizmos
	Entity selected_entity = scene_hierarchy_panel_.getSelectedEntity();
	if (selected_entity && gizmo_type_ != -1)
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		float window_width = (float)ImGui::GetWindowWidth();
		float window_height = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(
			viewport_bounds_[0].x,
			viewport_bounds_[0].y,
			viewport_bounds_[1].x - viewport_bounds_[0].x,
			viewport_bounds_[1].y - viewport_bounds_[0].y
		);

		// Camera
		//auto camera_entity = active_scene_->getPrimaryCameraEntity();
		//const auto& camera = camera_entity.getComponent<CameraComponent>().camera_;
		//const glm::mat4& camera_projection = camera.getProjection();
		//glm::mat4 camera_view = glm::inverse(camera_entity.getComponent<TransformComponent>().getTransform());

		const glm::mat4& camera_projection = editor_camera_.getProjection();
		glm::mat4 camera_view = editor_camera_.getViewMatrix();

		// Entity transform
		auto& tc = selected_entity.getComponent<TransformComponent>();
		glm::mat4 transform = tc.getTransform();

		// Snapping
		bool snap = Input::isKeyPressed(Key::LeftControl);
		float snap_value = 0.5f; // Snap to 0.5m for translation/scale
		// Snap to 45 degrees for rotation
		if (gizmo_type_ == ImGuizmo::OPERATION::ROTATE)
			snap_value = 45.0f;

		float snap_values[3] = { snap_value, snap_value, snap_value };

		ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
			(ImGuizmo::OPERATION)gizmo_type_, ImGuizmo::LOCAL, glm::value_ptr(transform),
			nullptr, snap ? snap_values : nullptr);

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 translation, rotation, scale;
			Math::DecomposeTransform(transform, translation, rotation, scale);

			glm::vec3 delta_rotation = rotation - tc.rotation_;
			tc.translation_ = translation;
			tc.rotation_ += delta_rotation;
			tc.scale_ = scale;
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();

	uiToolbar();

	ImGui::End();
}
