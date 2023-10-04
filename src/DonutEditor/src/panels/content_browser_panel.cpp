#include "pch.h"
#include "core/core.h"

#include "content_browser_panel.h"

#include "project/project.h"

#include "asset/texture_importer.h"

#include <imgui/imgui.h>

#include <filesystem>

namespace Donut
{
	//extern const std::filesystem::path g_asset_dir = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		: base_directory_(Project::getAssetDirectory()), current_dir_(Project::getAssetDirectory())
	{
		tree_nodes_.push_back(TreeNode("."));

		dir_icon_ = TextureImporter::loadTexture2D("assets/icons/content_browser/DirectoryIcon.png");
		file_icon_ = TextureImporter::loadTexture2D("assets/icons/content_browser/FileIcon.png");

		refreshAssetTree();

		mode_ = Mode::FileSystem;
	}

	void ContentBrowserPanel::onImGuiRender()
	{
		ImGui::Begin("Content Browser");

		const char* label = mode_ == Mode::Asset ? "Asset" : "File";
		if (ImGui::Button(label))
		{
			mode_ = mode_ == Mode::Asset ? Mode::FileSystem : Mode::Asset;
		}

		if (current_dir_ != std::filesystem::path(base_directory_))
		{
			ImGui::SameLine();
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
		if (mode_ == Mode::Asset)
		{
			TreeNode* node = &tree_nodes_[0];

			auto current_dir = std::filesystem::relative(current_dir_, Project::getAssetDirectory());
			for (const auto& p : current_dir_)
			{
				// if only one level
				if (node->path_ == current_dir)
				{
					break;
				}

				if (node->children_.find(p) != node->children_.end())
				{
					node = &tree_nodes_[node->children_[p]];
					continue;
				}
				else
				{
					// cannot find path
					DN_CORE_ASSERT(false);
				}
			}

			for (const auto& [item, tree_node_index] : node->children_)
			{
				bool is_directory = std::filesystem::is_directory(Project::getAssetDirectory() / item);

				std::string item_str = item.generic_string();

				ImGui::PushID(item_str.c_str());
				Ref<Texture2D> icon = is_directory ? dir_icon_ : file_icon_;
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton((ImTextureID)icon->getObjectId(), { thumnail_size, thumnail_size }, { 0, 1 }, { 1,0 });

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Import"))
					{
						//auto relative_path = std::filesystem::relative(Project::getAssetDirectory() / item, Project::getAssetDirectory());
						auto relative_path = Project::getAssetDirectory() / node->path_ / item;
						Project::getActive()->getEditorAssetManager()->importAsset(relative_path);
					}
					ImGui::EndPopup();
				}

				if (ImGui::BeginDragDropSource())
				{
					//std::filesystem::path relative_path(path);
					auto relative_path = Project::getAssetDirectory() / node->path_ / item;
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
					if (is_directory)
					{
						current_dir_ /= item.filename();
					}
				}

				ImGui::TextWrapped(item_str.c_str());

				ImGui::NextColumn();

				ImGui::PopID();

			}
		}
		else
		{
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
		}



		ImGui::Columns(1);

		ImGui::SliderFloat("Thumnail Size", &thumnail_size, 16, 512);
		ImGui::SliderFloat("Padding", &padding, 0, 32);

		ImGui::End();
	}


	void ContentBrowserPanel::refreshAssetTree()
	{
		const auto& asset_registry = Project::getActive()->getEditorAssetManager()->getAssetRegistry();
		for (const auto& [handle, metadata] : asset_registry)
		{
			uint32_t current_node_index = 0;
			for (const auto& p : metadata.file_path_)
			{
				auto it = tree_nodes_[current_node_index].children_.find(p.generic_string());
				if (it != tree_nodes_[current_node_index].children_.end())
				{
					current_node_index = it->second;
				}
				else
				{
					// add node
					TreeNode new_node(p);
					new_node.parent_ = current_node_index;
					tree_nodes_.push_back(new_node);

					tree_nodes_[current_node_index].children_[p] = tree_nodes_.size() - 1;
					current_node_index = tree_nodes_.size() - 1;
				}
			}
		}
	}
}