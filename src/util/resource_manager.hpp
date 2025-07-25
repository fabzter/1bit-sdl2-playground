#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <SDL2/SDL.h>
#include "sprite_asset_loader.hpp"
#include "tileset_asset.hpp"

// Forward-declare the custom deleter
struct SDL_Texture_Deleter;

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    // Disallow copying and moving
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    /**
     * @brief Loads an asset from file or gets it from the cache if already loaded.
     * @attention THIS SHOULD ONLY BE CALLED DURING A LOADING PHASE.
     */
    const SpriteAsset* loadSpriteAsset(SDL_Renderer* renderer, const std::string& assetId);

    /**
     * @brief Gets a pre-loaded asset from the cache. Does not load from file.
     * @attention THIS IS THE SAFE METHOD TO CALL DURING THE GAME LOOP (UPDATE/RENDER).
     * It is marked 'const' because it does not modify the resource manager's state.
     */
    const SpriteAsset* getSpriteAsset(const std::string& assetId) const;

    const TilesetAsset* loadTilesetAsset(SDL_Renderer* renderer, const std::string& assetId, const std::string& sourceHint = "");
    const TilesetAsset* getTilesetAsset(const std::string& assetId) const;

    [[nodiscard]] const std::string& getBasePath() const { return m_basePath; }

private:
    std::string m_basePath;
    std::unordered_map<std::string, std::unique_ptr<SpriteAsset>> m_spriteAssetCache;
    std::unordered_map<std::string, std::unique_ptr<TilesetAsset>> m_tilesetAssetCache;
};
