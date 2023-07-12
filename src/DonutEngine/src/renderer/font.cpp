#include "pch.h"
#include "font.h"

#include "renderer/msdf_data.h"

#undef INFINITE
#include <msdf-atlas-gen.h>

#include <msdfgen.h>
#include <msdfgen-ext.h>
#include <msdf-atlas-gen/GlyphGeometry.h>
#include <msdf-atlas-gen/FontGeometry.h>

using namespace msdfgen;

namespace Donut
{
	//struct MSDFData
	//{
	//	std::vector<msdf_atlas::GlyphGeometry> glyphs_;
	//	msdf_atlas::FontGeometry font_geometry_;
	//};

	template<typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
	static Ref<Texture2D> createAndCacheAtlas(const std::string& font_name, float font_size,
		const std::vector<msdf_atlas::GlyphGeometry>& glyphs,
		const msdf_atlas::FontGeometry& font_geometry, uint32_t width, uint32_t height)
	{
		msdf_atlas::GeneratorAttributes attributes;
		attributes.config.overlapSupport = true;
		attributes.scanlinePass = true;

		msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(width, height);
		generator.setAttributes(attributes);
		generator.setThreadCount(8);
		generator.generate(glyphs.data(), (int)glyphs.size());

		msdfgen::BitmapConstRef<T, N> bitmap = (msdfgen::BitmapConstRef<T, N>)generator.atlasStorage();

		TextureSpecification spec;
		spec.width_ = bitmap.width;
		spec.height_ = bitmap.height;
		spec.format_ = ImageFormat::RGB8;
		spec.generate_mips_ = false;

		Ref<Texture2D> texture = Texture2D::createTexture(spec);
		texture->setData((void*)bitmap.pixels, bitmap.width * bitmap.height * 3);
		return texture;

	}

	Font::Font(const std::filesystem::path& filepath)
		: msdf_data_(new MSDFData())
	{
		msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
		DN_CORE_ASSERT(ft, "initializeFreetype failed!");

		std::string file_string = filepath.string();

		msdfgen::FontHandle* font = msdfgen::loadFont(ft, file_string.c_str());
		if (!font)
		{
			DN_CORE_ERROR("failed to load font : {}", file_string);
			return;
		}

		struct CharsetRange
		{
			uint32_t begin_, end_;
		};

		// from imgui_draw.cpp
		static const CharsetRange charset_ranges[] =
		{
			{0x0020, 0x00ff}	// Basic Latin + Latin Supplement
		};

		msdf_atlas::Charset charset;

		for (auto range : charset_ranges)
		{
			for (uint32_t c = range.begin_; c <= range.end_; c++)
			{
				charset.add(c);
			}
		}

		double font_scale = 1.0;
		msdf_data_->font_geometry_ = msdf_atlas::FontGeometry(&msdf_data_->glyphs_);
		int glyphs_loaded = msdf_data_->font_geometry_.loadCharset(font, font_scale, charset);
		DN_CORE_INFO("loaded {} glyphs from font (out of {})", glyphs_loaded, charset.size());

		double em_size = 40.0;
		msdf_atlas::TightAtlasPacker atlas_packer;
		atlas_packer.setPixelRange(2.0);
		atlas_packer.setMiterLimit(1.0);
		atlas_packer.setPadding(0);
		atlas_packer.setScale(em_size);

		int remaining = atlas_packer.pack(msdf_data_->glyphs_.data(), (int)msdf_data_->glyphs_.size());
		DN_CORE_ASSERT(remaining == 0, "remaining == 0");

		int width, height;
		atlas_packer.getDimensions(width, height);
		em_size = atlas_packer.getScale();

#define DEFAULT_ANGLE_THRESHOLD 3.0
#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull
#define THREAD_COUNT 8
		// if MSDF || MTSDF

		uint64_t coloring_seed = 0;
		bool id_expensive_coloring = false;
		if (id_expensive_coloring)
		{
			msdf_atlas::Workload([&glyphs = msdf_data_->glyphs_, &coloring_seed](int i, int threadNo) -> bool {
				unsigned long long glyph_seed = (LCG_MULTIPLIER * (coloring_seed ^ i) + LCG_INCREMENT) * !!coloring_seed;
			glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyph_seed);
			return true;
				}, msdf_data_->glyphs_.size()).finish(THREAD_COUNT);
		}
		else {
			unsigned long long glyph_seed = coloring_seed;
			for (msdf_atlas::GlyphGeometry& glyph : msdf_data_->glyphs_)
			{
				glyph_seed *= LCG_MULTIPLIER;
				glyph.edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyph_seed);
			}
		}

		atlas_texture_ = createAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>(
			"test",
			(float)em_size,
			msdf_data_->glyphs_,
			msdf_data_->font_geometry_,
			width, height
		);

		msdfgen::destroyFont(font);
		msdfgen::deinitializeFreetype(ft);
	}

	Font::~Font()
	{
		if (msdf_data_)
		{
			delete msdf_data_;
		}
	}
}