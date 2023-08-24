#include "pch.h"
#include "project_serializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Donut
{
	ProjectSerializer::ProjectSerializer(Ref<Project> project)
		: project_(project)
	{
	}

	bool ProjectSerializer::serialize(const std::filesystem::path& filepath)
	{
		const auto& config = project_->getConfig();

		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "Project" << YAML::Value;
			{
				out << YAML::BeginMap;// Project
				out << YAML::Key << "Name" << YAML::Value << config.name_;
				out << YAML::Key << "StartScene" << YAML::Value << config.start_scene_.string();
				out << YAML::Key << "AssetDirectory" << YAML::Value << config.asset_directory_.string();
				out << YAML::Key << "AssetRegistryPath" << YAML::Value << config.asset_registry_path_.string();
				out << YAML::Key << "ScriptModulePath" << YAML::Value << config.script_module_path_.string();
				out << YAML::EndMap; // Project
			}
			out << YAML::EndMap; // Root
		}

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}

	bool ProjectSerializer::deserialize(const std::filesystem::path& filepath)
	{
		auto& config = project_->getConfig();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			DN_CORE_ERROR("Failed to load project file '{0}'\n     {1}", filepath, e.what());
			return false;
		}

		auto project_node = data["Project"];
		if (!project_node)
			return false;

		config.name_ = project_node["Name"].as<std::string>();
		config.start_scene_ = project_node["StartScene"].as<std::string>();
		config.asset_directory_ = project_node["AssetDirectory"].as<std::string>();
		if (project_node["AssetRegistryPath"])
		{
			config.asset_registry_path_ = project_node["AssetRegistryPath"].as<std::string>();
		}
		config.script_module_path_ = project_node["ScriptModulePath"].as<std::string>();
		return true;
	}
}