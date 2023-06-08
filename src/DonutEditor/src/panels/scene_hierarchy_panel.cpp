#include "scene_hierarchy_panel.h"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "../DonutEngine/src/scene/components.h"

namespace Donut
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
	{
		setContext(scene);
	}

	void SceneHierarchyPanel::setContext(const Ref<Scene>& scene)
	{
		context_ = scene;
	}

	void SceneHierarchyPanel::onImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		context_->registry_.each([&](auto entity_id)
			{
				Entity entity{ entity_id, context_.get() };
				drawEntityNode(entity);
			});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			selection_context_ = {};
		}

		ImGui::End();

		ImGui::Begin("Properties");

		if (selection_context_)
		{
			drawComponents(selection_context_);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::drawComponents(Entity entity)
	{
		if (entity.hasComponent<TagComponent>())
		{
			auto& tag = entity.getComponent<TagComponent>().tag_;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		if (entity.hasComponent<TransformComponent>())
		{
			if (ImGui::TreeNodeEx((void*)(typeid(TransformComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				auto& transform = entity.getComponent<TransformComponent>().transform_;
				ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);

				ImGui::TreePop();
			}
		}

		if (entity.hasComponent<CameraComponent>())
		{
			if (ImGui::TreeNodeEx((void*)(typeid(CameraComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				auto& camera_component = entity.getComponent<CameraComponent>();
				auto& camera = camera_component.camera_;

				ImGui::Checkbox("Is Primary Camera", &camera_component.is_primary_);

				const char* projection_type[] = { "Perspective", "Orthographic" };
				const char* current_projection_type = projection_type[(int)camera.getProjectionType()];
				if (ImGui::BeginCombo("Projection", current_projection_type))
				{
					for (int i = 0; i < 2; i++)
					{
						// is_selected 设置点击扩展后是否高亮显示
						bool is_selected = current_projection_type == projection_type[i];
						if (ImGui::Selectable(projection_type[i], is_selected))
						{
							current_projection_type = projection_type[i];
							camera.setProjectionType((SceneCamera::ProjectionType)i);
						}

						// 暂时不知有什么作用
						if (is_selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if(camera_component.camera_.getProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float ortho_size = camera.getOrthographicSize();
					if (ImGui::DragFloat("Size", &ortho_size), 0.1f)
					{
						camera.setOrthographicSize(ortho_size);
					}

					float ortho_near = camera.getOrthographicNearClip();
					if (ImGui::DragFloat("Near", &ortho_near), 0.1f)
					{
						camera.setOrthographicNearClip(ortho_near);
					}

					float ortho_far = camera.getOrthographicFarClip();
					if (ImGui::DragFloat("Far", &ortho_far), 0.1f)
					{
						camera.setOrthographicFarClip(ortho_far);
					}

					ImGui::Checkbox("Is Fixed Aspect Ratio", &camera_component.is_fixed_aspect_ratio_);
				}

				if (camera_component.camera_.getProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float perspective_fov = glm::degrees(camera.getPerspectiveVerticalFov());
					if (ImGui::DragFloat("FOV", &perspective_fov), 0.1f)
					{
						camera.setPerspectiveVerticalFov(perspective_fov);
					}

					float perspective_near = camera.getPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &perspective_near), 0.1f)
					{
						camera.setPerspectiveNearClip(perspective_near);
					}

					float perspective_far = camera.getPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &perspective_far), 0.1f)
					{
						camera.setPerspectiveFarClip(perspective_far);
					}
				}

				ImGui::TreePop();
			}
		}

	}

	void SceneHierarchyPanel::drawEntityNode(Entity& entity)
	{
		auto& tag = entity.getComponent<TagComponent>().tag_;

		ImGuiTreeNodeFlags flags = ((selection_context_ == entity) ? ImGuiTreeNodeFlags_Selected : 0 )| ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)((uint64_t)(uint32_t)entity),flags,tag.c_str());
		if (ImGui::IsItemClicked())
		{
			selection_context_ = entity;
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ((selection_context_ == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)((uint64_t)(uint32_t)entity + 1000), flags, tag.c_str());
			if (opened)
			{
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}
	}
}