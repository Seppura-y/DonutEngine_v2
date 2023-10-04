#ifndef EDITOR_ASSET_MANAGER_H
#define EDITOR_ASSET_MANAGER_H

#include "asset_manager_base.h"
#include "asset_metadata.h"

namespace Donut
{
	using AssetMap = std::map<AssetHandle, Ref<Asset>>;
	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;
	using AssetFilepath = std::map<std::string, AssetHandle>;

	class EditorAssetManager : public AssetManagerBase
	{
	public:
		virtual Ref<Asset> getAsset(AssetHandle handle) const override;

		virtual bool isAssetHandleValid(AssetHandle handle) const override;
		virtual bool isAssetLoaded(AssetHandle handle) const override;

		const AssetMetadata& getMetadata(AssetHandle handle) const;

		void importAsset(const std::filesystem::path& filepath);

		const AssetRegistry& getAssetRegistry() const { return asset_registry_; }

		void serializeAssetRegistry();
		bool deserializeAssetRegistry();

		virtual AssetHandle getAssetHandleFromFilePath(const std::string& filepath) override;

	private:
		AssetRegistry asset_registry_;
		AssetMap loaded_assets_;
		AssetFilepath filepath_to_assets_;
	};
}

#endif