#ifndef CONTENT_BROWSER_PANEL_H
#define CONTENT_BROWSER_PANEL_H

#include "renderer/texture.h"

#include <filesystem>
#include <map>
#include <set>

namespace Donut
{
    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel();

        void onImGuiRender();

    private:
        void refreshAssetTree();
    private:
        std::filesystem::path current_dir_;
        std::filesystem::path base_directory_;

        Ref<Texture2D> dir_icon_;
        Ref<Texture2D> file_icon_;

        struct TreeNode
        {
            std::filesystem::path path_;
            uint32_t parent_ = (uint32_t)-1;
            std::map<std::filesystem::path, uint32_t> children_;

            TreeNode(const std::filesystem::path& path)
                : path_(path){}
        };

        std::vector<TreeNode> tree_nodes_;
        std::map<std::filesystem::path, std::vector<std::filesystem::path>> asset_tree_;

        enum class Mode
        {
            Asset = 0, FileSystem = 1
        };

        Mode mode_ = Mode::Asset;
    };
}
#endif