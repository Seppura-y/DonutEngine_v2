#ifndef CONTENT_BROWSER_PANEL_H
#define CONTENT_BROWSER_PANEL_H

#include <filesystem>

#include "renderer/texture.h"

namespace Donut
{
    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel();

        void onImGuiRender();

    private:
        std::filesystem::path current_dir_;
        std::filesystem::path base_directory_;

        Ref<Texture2D> dir_icon_;
        Ref<Texture2D> file_icon_;
    };
}
#endif