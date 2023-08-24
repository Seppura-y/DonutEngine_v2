#ifndef TEXTURE_IMPORTER_H
#define TEXTURE_IMPORTER_H

#include "asset.h"
#include "asset_metadata.h"

#include "renderer/texture.h"

namespace Donut
{
	class TextureImporter
	{
	public:
		static Ref<Texture2D> importTexture2D(AssetHandle handle, const AssetMetadata& metadata);

		static Ref<Texture2D> loadTexture2D(const std::filesystem::path& path);
	};
}

#endif