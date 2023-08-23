#ifndef ASSET_MANAGER_BASE_H
#define ASSET_MANAGER_BASE_H

#include "asset.h"

#include <map>

namespace Donut
{
	using AssetMap = std::map<AssetHandle, Ref<Asset>>;

	class AssetManagerBase
	{
	public:
		virtual Ref<Asset> getAsset(AssetHandle handle) const = 0;

		virtual bool isAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool isAssetLoaded(AssetHandle handle) const = 0;
	};
}

#endif