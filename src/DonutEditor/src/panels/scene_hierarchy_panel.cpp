#include "scene_hierarchy_panel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

#include "../DonutEngine/src/scene/components.h"

namespace Donut
{
	static void drawVec3Control(const std::string& label, glm::vec3& values, float reset_value = 0.0f, float colum_width = 100.0f)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, colum_width);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 button_size = { line_height + 3.0f, line_height };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.25f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("X", button_size))
		{
			values.x = reset_value;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		// Note : "##X" is the identifier, need to be unique, otherwise the drag movement will affect multi-items
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();



		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.15f, 0.8f, 0.1f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.25f, 0.9f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.15f, 0.8f, 0.1f, 1.0f });
		if (ImGui::Button("Y", button_size))
		{
			values.y = reset_value;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();



		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.15f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.25f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.15f, 0.8f, 1.0f });
		if (ImGui::Button("Z", button_size))
		{
			values.z = reset_value;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

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

		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				context_->createEntity("Empty Entity");
			}

			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");

		if (selection_context_)
		{
			drawComponents(selection_context_);

			if (ImGui::Button("Add Component"))
			{
				ImGui::OpenPopup("AddComponent");
			}

			if (ImGui::BeginPopup("AddComponent"))
			{
				if (ImGui::MenuItem("Camera"))
				{
					selection_context_.addComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Transform"))
				{
					selection_context_.addComponent<TransformComponent>();
					ImGui::CloseCurrentPopup();
				}

				if (ImGui::MenuItem("Sprite Renderer"))
				{
					selection_context_.addComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::drawComponents(Entity entity)
	{
		// allow overlap to draw a overlap button which is used to delete the overlapped component
		const ImGuiTreeNodeFlags treenode_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

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
			bool is_opened = ImGui::TreeNodeEx((void*)(typeid(TransformComponent).hash_code()), treenode_flags, "TransformComponent");
			ImGui::SameLine();
			if (ImGui::Button("+"))
			{
				// the string that as parameter is only a identifier, not the text to render in the UI
				ImGui::OpenPopup("ComponentSettings");
			}

			bool component_removed = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
				{
					component_removed = true;
				}
				ImGui::EndPopup();
			}

			if (is_opened)
			{
				auto& transform_component = entity.getComponent<TransformComponent>();
				//ImGui::DragFloat3("Position", glm::value_ptr(transform_component.translation_), 0.1f);
				drawVec3Control("Translation", transform_component.translation_);
				auto degree = glm::degrees(transform_component.rotation_);
				drawVec3Control("Rotation", degree);
				transform_component.rotation_ = glm::radians(degree);

				drawVec3Control("Scale", transform_component.scale_);

				ImGui::TreePop();
			}

			if (component_removed)
			{
				entity.removeComponent<TransformComponent>();
			}
		}

		if (entity.hasComponent<CameraComponent>())
		{
			bool is_opened = ImGui::TreeNodeEx((void*)(typeid(CameraComponent).hash_code()), treenode_flags, "CameraComponent");

			if (is_opened)
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

		if (entity.hasComponent<SpriteRendererComponent>())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4,4 });
			bool is_opened = ImGui::TreeNodeEx((void*)(typeid(SpriteRendererComponent).hash_code()), treenode_flags, "SpriteRendererComponent");
			ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
			if (ImGui::Button("+"))
			{
				// the string that as parameter is only a identifier, not the text to render in the UI
				ImGui::OpenPopup("ComponentSettings");
			}
			ImGui::PopStyleVar();

			bool component_removed = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
				{
					component_removed = true;
				}
				ImGui::EndPopup();
			}

			if (is_opened)
			{
				auto& sprite_renderer_component = entity.getComponent<SpriteRendererComponent>();
				auto& color = sprite_renderer_component.color;
				ImGui::ColorEdit4("SpriteColor", glm::value_ptr(color));

				ImGui::TreePop();
			}

			if (component_removed)
			{
				entity.removeComponent<SpriteRendererComponent>();
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

		bool entity_deleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				//context_->destroyEntity(entity);
				entity_deleted = true;
			}

			ImGui::EndPopup();

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

		if (entity_deleted)
		{
			context_->destroyEntity(entity);
			if (selection_context_ == entity)
			{
				selection_context_ = {};
			}
		}
	}
}