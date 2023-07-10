#include "pch.h"
#include "content_browser_panel.h"

#include "project/project.h"

#include <imgui/imgui.h>

#include <filesystem>

namespace Donut
{
	//extern const std::filesystem::path g_asset_dir = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		: base_directory_(Project::getAssetDirectory()), current_dir_(Project::getAssetDirectory())
	{
		dir_icon_ = Texture2D::createTexture("assets/icons/content_browser/DirectoryIcon.png");
		file_icon_ = Texture2D::createTexture("assets/icons/content_browser/FileIcon.png");
	}

	void ContentBrowserPanel::onImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (current_dir_ != std::filesystem::path(base_directory_))
		{
			if (ImGui::Button("<-"))
			{
				current_dir_ = current_dir_.parent_path();
			}
		}

		static float padding = 16.0f;
		static float thumnail_size = 128.0f;
		float cell_size = thumnail_size + padding;

		float panel_width = ImGui::GetContentRegionAvail().x;
		int colum_count = (int)(panel_width / cell_size);
		if (colum_count < 1)
		{
			colum_count = 1;
		}

		ImGui::Columns(colum_count, 0, false);

		for (auto& dir_entry : std::filesystem::directory_iterator(current_dir_))
		{
			const auto& path = dir_entry.path();
			//auto relative_path = std::filesystem::relative(path, g_asset_dir);

			// 逐个层级带斜杠的路径
			//std::string filename = path.string();

			// 只有当前路径，不带父目录
			//std::string filename = relative_path.filename().string();

			std::string filename = path.filename().string();

			ImGui::PushID(filename.c_str());
			Ref<Texture2D> icon = dir_entry.is_directory() ? dir_icon_ : file_icon_;

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->getObjectId(), { thumnail_size, thumnail_size }, { 0,1 }, { 1,0 });
			
			if (ImGui::BeginDragDropSource())
			{
				std::filesystem::path relative_path(path);
				const wchar_t* item_path = relative_path.c_str();

				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM",
					item_path,
					(wcslen(item_path) + 1) * sizeof(wchar_t) // NTCTS: null-terminated-character-type-string
				);
				
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (dir_entry.is_directory())
				{
					current_dir_ /= path.filename();
				}
			}

			ImGui::TextWrapped(filename.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::SliderFloat("Thumnail Size", &thumnail_size, 16, 512);
		ImGui::SliderFloat("Padding", &padding, 0, 32);

		ImGui::End();
	}
}