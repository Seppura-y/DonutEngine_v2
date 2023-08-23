#ifndef PROJECT_H
#define PROJECT_H


#include "core/core.h"

#include <string>
#include <filesystem>

namespace Donut
{
	struct ProjectConfig
	{
		std::string name_ = "Untitled";

		std::filesystem::path start_scene_;

		std::filesystem::path asset_directory_;
		std::filesystem::path script_module_path_;
	};

	class Project
	{
	public:
		static const std::filesystem::path& getProjectDirectory()
		{
			DN_CORE_ASSERT(active_project_, " getProjectDirectory ");
			return active_project_->project_directory_;
		}

		static std::filesystem::path getAssetDirectory()
		{
			DN_CORE_ASSERT(active_project_, "getAssetDirectory");
			//return getProjectDirectory() / active_project_->config_.asset_directory_;
			return getProjectDirectory().parent_path();
		}

		// TODO(Yan): move to asset manager when we have one
		static std::filesystem::path getAssetFileSystemPath(const std::filesystem::path& path)
		{
			DN_CORE_ASSERT(active_project_, "getAssetFileSystemPath");
			return std::filesystem::canonical(path);
			//return getAssetDirectory() / path;
		}

		ProjectConfig& getConfig() { return config_; }

		static Ref<Project> getActive() { return active_project_; }

		static Ref<Project> newProject();
		static Ref<Project> loadProject(const std::filesystem::path& path);
		static bool saveActive(const std::filesystem::path& path);
	private:
		ProjectConfig config_;
		std::filesystem::path project_directory_;

		inline static Ref<Project> active_project_;
	};

}

#endif