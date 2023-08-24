#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "asset_manager_base.h"
#include "project/project.h"

namespace Donut
{
	class AssetManager
	{
	public:
		template<typename T>
		static Ref<T> getAsset(AssetHandle handle)
		{
			Ref<Asset> asset = Project::getActive()->getAssetManager()->getAsset(handle);
			//Ref<Asset> asset = Project::getActive();
			return std::static_pointer_cast<T>(asset);
		}
	};
}

#endif