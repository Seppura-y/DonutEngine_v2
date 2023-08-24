#ifndef ASSET_H
#define ASSET_H

#include "core/core.h"
#include "core/uuid.h"

#include <stdint.h>

namespace Donut
{
	using AssetHandle = UUID;

	enum class AssetType : uint16_t
	{
		None = 0,
		Scene,
		Texture2D
	};

	std::string_view assetTypeToString(AssetType type);
	AssetType assetTypeFromString(std::string_view type);

	class Asset
	{
	public:
		AssetHandle handle_;
		virtual AssetType getType() const = 0;
	};
}

#endif