#include "pch.h"
#include "project.h"

#include "project_serializer.h"

namespace Donut
{
	Ref<Project> Project::newProject()
	{
		active_project_ = createRef<Project>();
		return active_project_;
	}

	Ref<Project> Project::loadProject(const std::filesystem::path& path)
	{
		Ref<Project> project = createRef<Project>();

		ProjectSerializer serializer(project);
		if (serializer.deserialize(path))
		{
			project->project_directory_ = path.parent_path();
			active_project_ = project;

			std::shared_ptr<EditorAssetManager> editor_asset_manager = std::make_unique<EditorAssetManager>();
			active_project_->asset_manager_ = editor_asset_manager;
			editor_asset_manager->deserializeAssetRegistry();

			return active_project_;
		}

		return nullptr;
	}

	bool Project::saveActive(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(active_project_);
		if (serializer.serialize(path))
		{
			active_project_->project_directory_ = path.parent_path();
			return true;
		}

		return false;
	}
}