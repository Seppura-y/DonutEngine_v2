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