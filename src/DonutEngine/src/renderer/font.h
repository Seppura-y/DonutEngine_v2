#ifndef FONT_H
#define FONT_H

#include <filesystem>

namespace Donut
{
	class Font
	{
	public:
		Font(const std::filesystem::path& font);
	};
}

#endif