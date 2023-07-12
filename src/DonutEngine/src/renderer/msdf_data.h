#ifndef MSDF_DATA_H
#define MSDF_DATA_H

#include <vector>

#undef INFINITE
#include <msdf-atlas-gen.h>

namespace Donut
{
	struct MSDFData
	{
		std::vector<msdf_atlas::GlyphGeometry> glyphs_;
		msdf_atlas::FontGeometry font_geometry_;
	};
}


#endif