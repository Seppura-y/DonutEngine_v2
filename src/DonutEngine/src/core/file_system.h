#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "core/buffer.h"

#include <filesystem>

namespace Donut
{
	class FileSystem
	{
	public:
		static Buffer readFileBinary(const std::filesystem::path& filepath);
	};
}
#endif