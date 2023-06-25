#ifndef PLATFORM_UTILS_H
#define PLATFORM_UTILS_H

#include <string>

namespace Donut
{
	class FileDialogs
	{
	public:
		static std::string openFile(const char* filter);
		static std::string saveFile(const char* filter);
	private:

	};

	class Time
	{
	public:
		static float getTime();
	};
}

#endif