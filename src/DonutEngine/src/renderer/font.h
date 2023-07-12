#ifndef FONT_H
#define FONT_H

#include "renderer/texture.h"

#include <filesystem>

namespace Donut
{
	struct MSDFData;

	class Font
	{
	public:
		Font(const std::filesystem::path& font);
		~Font();

		Ref<Texture2D> getAtlasTexture() const { return atlas_texture_; }
	private:
		MSDFData* msdf_data_;
		Ref<Texture2D> atlas_texture_;
	};
}

#endif