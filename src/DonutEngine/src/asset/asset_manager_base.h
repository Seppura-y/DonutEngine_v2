#ifndef ASSET_MANAGER_BASE_H
#define ASSET_MANAGER_BASE_H

#include "asset.h"

#include <map>

namespace Donut
{

	class AssetManagerBase
	{
	public:
		virtual Ref<Asset> getAsset(AssetHandle handle) const = 0;

		virtual bool isAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool isAssetLoaded(AssetHandle handle) const = 0;


		virtual AssetHandle getAssetHandleFromFilePath(const std::string& filepath) = 0;
	};
}

#endif