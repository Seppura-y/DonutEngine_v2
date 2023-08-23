#include "pch.h"
#include "asset_manager.h"
#include "asset_importer.h"
#include "editor_asset_manager.h"

namespace Donut
{
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