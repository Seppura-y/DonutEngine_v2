#ifndef CONTENT_BROWSER_PANEL_H
#define CONTENT_BROWSER_PANEL_H

#include <filesystem>

namespace Donut
{
    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel();

        void onImGuiRender();

    private:
        std::filesystem::path current_dir_;
    };
}
#endif