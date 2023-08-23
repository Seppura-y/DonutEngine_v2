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

	class Asset
	{
	public:
		AssetHandle handle_;
		virtual AssetType getType() const = 0;
	};
}

#endif