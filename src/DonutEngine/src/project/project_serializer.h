#ifndef PROJECT_SERIALIZER_H
#define PROJECT_SERIALIZER_H

#include "project.h"

namespace Donut
{
	class ProjectSerializer
	{
	public:
		ProjectSerializer(Ref<Project> project);

		bool serialize(const std::filesystem::path& filepath);
		bool deserialize(const std::filesystem::path& filepath);
	private:
		Ref<Project> project_;
	};
}

#endif