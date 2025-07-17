#include "resource_manager.hpp"
#include <iostream>

void SDL_Texture_Deleter::operator()(SDL_Texture* texture) const {
    SDL_DestroyTexture(texture);
}

ResourceManager::ResourceManager() {
    // SDL_GetBasePath() gives us the directory of our executable.
    // This is the key to finding our resources correctly.
    char* basePath = SDL_GetBasePath();
    if (basePath) {
        m_basePath = basePath;
        SDL_free(basePath);
    } else {
        std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
        m_basePath = "./"; // Fallback to relative path
    }
}

ResourceManager::~ResourceManager() = default; // Smart pointers handle cleanup

const SpriteAsset *ResourceManager::getSpriteAsset(SDL_Renderer *renderer, const std::string &assetId) {
    auto it = m_spriteAssetCache.find(assetId);
    if (it != m_spriteAssetCache.end()) {
        return it->second.get();
    }

    std::string fullPath = m_basePath + "res/sprites/" + assetId + ".sprite";
    std::cout << "Loading sprite asset from: " << fullPath << std::endl;

    std::unique_ptr<SpriteAsset> asset = SpriteAssetLoader::loadFromFile(renderer, fullPath);
    if (asset) {
        // We still use the requested assetId as the key for the cache.

        // Enforce that the ID in the file matches the requested assetId.
        if (asset->assetId != assetId) {
            std::cerr << "Asset ID Mismatch! File '" << fullPath
                      << "' has SPRITE_NAME '" << asset->assetId
                      << "' but was requested with ID '" << assetId << "'." << std::endl;
            return nullptr; // Return nullptr to indicate a loading failure.
        }

        auto [inserted_it, success] = m_spriteAssetCache.emplace(assetId, std::move(asset));
        return inserted_it->second.get();
    }

    return nullptr;
}

void ResourceManager::preloadSpriteAssets(SDL_Renderer* renderer, const std::vector<std::string>& assetIds) {
    std::cout << "Preloading " << assetIds.size() << " sprite asset(s)..." << std::endl;
    for (const auto& assetId : assetIds) {
        getSpriteAsset(renderer, assetId);
    }
    std::cout << "Preloading complete." << std::endl;
}