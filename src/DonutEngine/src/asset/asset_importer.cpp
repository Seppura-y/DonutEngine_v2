#include "pch.h"
#include "asset_importer.h"

#include "texture_importer.h"

#include <map>

namespace Donut
{
	using AssetImportFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;

	static std::map<AssetType, AssetImportFunction> s_asset_import_functions =
	{
		{ AssetType::Texture2D, TextureImporter::importTexture2D }
	};

	Ref<Asset> AssetImporter::importAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_asset_import_functions.find(metadata.type_) == s_asset_import_functions.end())
		{
			DN_CORE_ASSERT("No importer avaliable for asset type : {}", (uint16_t)metadata.type_);
			return nullptr;
		}
		return s_asset_import_functions.at(metadata.type_)(handle, metadata);
	}
}