#include "pch.h"
#include "content_browser_panel.h"

#include <imgui/imgui.h>

#include <filesystem>

namespace Donut
{
	static const std::filesystem::path s_asset_dir = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		: current_dir_(s_asset_dir)
	{
	}

	void ContentBrowserPanel::onImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (current_dir_ != std::filesystem::path(s_asset_dir))
		{
			if (ImGui::Button("<-"))
			{
				current_dir_ = current_dir_.parent_path();
			}
		}

		for (auto& dir_entry : std::filesystem::directory_iterator(current_dir_))
		{
			const auto& path = dir_entry.path();
			auto relative_path = std::filesystem::relative(path, s_asset_dir);

			// 逐个层级带斜杠的路径
			//std::string filename = path.string();

			// 只有当前路径，不带父目录
			std::string filename = relative_path.filename().string();

			if (dir_entry.is_directory())
			{
				if (ImGui::Button(filename.c_str()))
				{
					current_dir_ /= path.filename();
				}
			}
			else
			{
				if (ImGui::Button(filename.c_str()))
				{

				}
			}
		}

		ImGui::End();
	}
}