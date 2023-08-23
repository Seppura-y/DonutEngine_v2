#ifndef ASSET_IMPORTER_H
#define ASSET_IMPORTER_H

#include "asset_metadata.h"

namespace Donut
{
	class AssetImporter
	{
	public:
		static Ref<Asset> importAsset(AssetHandle handle, const AssetMetadata& metadata);
	};
}

#endif