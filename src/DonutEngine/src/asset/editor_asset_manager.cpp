#include "pch.h"
#include "asset_manager.h"
#include "asset_importer.h"
#include "editor_asset_manager.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Donut
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const std::string_view& v)
	{
		out << std::string(v.data(), v.size());
		return out;
	}


	bool EditorAssetManager::isAssetHandleValid(AssetHandle handle) const
	{
		return handle != 0 && asset_registry_.find(handle) != asset_registry_.end();
	}

	bool EditorAssetManager::isAssetLoaded(AssetHandle handle) const
	{
		return loaded_assets_.find(handle) != loaded_assets_.end();
	}

	const AssetMetadata& EditorAssetManager::getMetadata(AssetHandle handle) const
	{
		static AssetMetadata s_null_metadata;
		auto it = asset_registry_.find(handle);
		if (it == asset_registry_.end())
		{
			return s_null_metadata;
		}
		else
		{
			return it->second;
		}
	}

	void EditorAssetManager::importAsset(const std::filesystem::path& filepath)
	{
		AssetHandle handle;
		AssetMetadata metadata;
		metadata.file_path_ = filepath;
		metadata.type_ = AssetType::Texture2D;
		Ref<Asset> asset = AssetImporter::importAsset(handle, metadata);
		if (asset)
		{
			asset->handle_ = handle;
			loaded_assets_[handle] = asset;
			asset_registry_[handle] = metadata;
			filepath_to_assets_[filepath.string()] = handle;
			serializeAssetRegistry();
		}
	}

	void EditorAssetManager::serializeAssetRegistry()
	{
		auto path = Project::getAssetRegistryPath();

		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "AssetRegistry" << YAML::Value;

			out << YAML::BeginSeq;
			for (const auto& [handle, metadata] : asset_registry_)
			{
				std::string filepath_str = metadata.file_path_.generic_string();

				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << handle;
				out << YAML::Key << "FilePath" << YAML::Value << filepath_str;
				out << YAML::Key << "Type" << YAML::Value << assetTypeToString(metadata.type_);
				out << YAML::EndMap;
			}

			out << YAML::EndSeq;
			out << YAML::EndMap;
		}

		std::ofstream fout(path);
		fout << out.c_str();
	}

	bool EditorAssetManager::deserializeAssetRegistry()
	{
		auto path = Project::getAssetRegistryPath();
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException e)
		{
			DN_CORE_ERROR("Failed to load project file {0}\n    {1}", path, e.what());
			return false;
		}

		auto root_node = data["AssetRegistry"];
		if (!root_node)
		{
			return false;
		}

		for (const auto& node : root_node)
		{
			AssetHandle handle = node["Handle"].as<uint64_t>();
			auto& metadata = asset_registry_[handle];
			metadata.file_path_ = node["FilePath"].as<std::string>();

			filepath_to_assets_[node["FilePath"].as<std::string>()] = handle;

			metadata.type_ = assetTypeFromString(node["Type"].as<std::string>());
			metadata.type_ = AssetType::Texture2D;
		}

		return true;
	}

	AssetHandle EditorAssetManager::getAssetHandleFromFilePath(const std::string& filepath)
	{
		//std::string fixed_filepath = filepath;
		//std::replace(fixed_filepath.begin(), fixed_filepath.end(), '\\', '/');

		if (filepath_to_assets_.find(filepath) != filepath_to_assets_.end())
			return filepath_to_assets_[filepath];
	}

	Ref<Asset> Donut::EditorAssetManager::getAsset(AssetHandle handle) const
	{
		// 1. Check if handle is valid
		if (!isAssetHandleValid(handle))
		{
			return nullptr;
		}

		// 2. Check if asset needs load
		Ref<Asset> asset;
		if (isAssetLoaded(handle))
		{
			asset = loaded_assets_.at(handle);
		}
		else
		{
			const AssetMetadata& metadata = getMetadata(handle);
			asset = AssetImporter::importAsset(handle, metadata);
			if (!asset)
			{
				DN_CORE_ERROR("EditorAssetManager::getAsset - asset import failed!");
			}
		}

		// 3. return
		return asset;
		
	}

}