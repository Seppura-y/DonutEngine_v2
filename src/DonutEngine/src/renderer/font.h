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

		const MSDFData* getMSDFData() const { return msdf_data_; }
	private:
		MSDFData* msdf_data_;
		Ref<Texture2D> atlas_texture_;
	};
}

#endif