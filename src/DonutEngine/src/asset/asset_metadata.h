#ifndef ASSET_METADATA_H
#define ASSET_METADATA_H

#include "asset.h"

#include <filesystem>

namespace Donut
{
	struct AssetMetadata
	{
		AssetType type_ = AssetType::None;
		std::filesystem::path file_path_;

		operator bool() const { return type_ != AssetType::None; }
	};
}

#endif