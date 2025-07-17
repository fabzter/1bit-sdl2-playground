#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <SDL2/SDL.h>
#include "sprite_asset_loader.hpp"

// Forward-declare the custom deleter
struct SDL_Texture_Deleter;

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    // Disallow copying and moving
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    const SpriteAsset* getSpriteAsset(SDL_Renderer* renderer, const std::string& assetId);
    void preloadSpriteAssets(SDL_Renderer* renderer, const std::vector<std::string>& assetIds);

private:
    std::string m_basePath;
    std::unordered_map<std::string, std::unique_ptr<SpriteAsset>> m_spriteAssetCache;
};
