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