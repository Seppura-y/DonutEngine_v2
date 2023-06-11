#ifndef SCENE_HIERARCHY_PANEL_H
#define SCENE_HIERARCHY_PANEL_H

#include "core/core.h"
#include "core/logger.h"
#include "scene/scene.h"
#include "scene/entity.h"

namespace Donut
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void setContext(const Ref<Scene>& scene);

		void onImGuiRender();

		Entity getSelectedEntity() const { return selection_context_; }

	private:
		void drawComponents(Entity entity);
		void drawEntityNode(Entity& entity);
	private:
		Ref<Scene> context_;
		Entity selection_context_;
	};
}

#endif