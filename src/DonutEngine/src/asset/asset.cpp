#include "pch.h"
#include "asset.h"

namespace Donut
{
    std::string_view assetTypeToString(AssetType type)
    {
        switch (type)
        {
            case AssetType::None: return "AssetType::None";
            case AssetType::Scene: return "AssetType::Scene";
            case AssetType::Texture2D: return "AssetType::Texture2D";
        }
        return std::string_view();
    }
    AssetType assetTypeFromString(std::string_view type)
    {
        if (type == "AssetType::None") return AssetType::None;
        if (type == "AssetType::Scene") return AssetType::Scene;
        if (type == "AssetType::Texture2D") return AssetType::Texture2D;

        return AssetType::None;
    }
}