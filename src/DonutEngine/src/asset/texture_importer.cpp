#include "pch.h"
#include "texture_importer.h"
#include "buffer.h"

#include <stb_image.h>

namespace Donut
{
	Ref<Texture2D> TextureImporter::importTexture2D(AssetHandle handle, const AssetMetadata& metadata)
	{
		DN_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		Buffer data;

		{
			DN_PROFILE_SCOPE("stbi_load - TextureImporter::importTexture2D");
			std::string path_str = metadata.file_path_.string();
			data.data_ = stbi_load(path_str.c_str(), &width, &height, &channels, 0);
		}

		if (data.data_ == nullptr)
		{
			DN_CORE_ERROR("TextureImporter::importTexture2D - Could not load texture from filepath : {}", metadata.file_path_.string());
			return nullptr;
		}

		data.size_ = width * height * channels;

		TextureSpecification spec;
		spec.width_ = width;
		spec.height_ = height;
		switch (channels)
		{
		case 3:
			spec.format_ = ImageFormat::RGB8;
			break;
		case 4:
			spec.format_ = ImageFormat::RGBA8;
			break;
		}

		Ref<Texture2D> texture = Texture2D::createTexture(spec, data);
		data.release();
		return texture;
	}
}