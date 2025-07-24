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

const SpriteAsset* ResourceManager::getSpriteAsset(const std::string& assetId) const {
    auto it = m_spriteAssetCache.find(assetId);
    if (it != m_spriteAssetCache.end()) {
        return it->second.get();
    }
    // Return nullptr if the asset was not pre-loaded.
    return nullptr;
}

const SpriteAsset* ResourceManager::loadSpriteAsset(SDL_Renderer* renderer, const std::string& assetId) {
    // First, check the cache.
    if (auto* asset = getSpriteAsset(assetId)) {
        return asset;
    }

    // If not in cache, load from file.
    std::string fullPath = m_basePath + "res/sprites/" + assetId + ".sprite";
    std::cout << "Loading sprite asset from: " << fullPath << std::endl;

    std::unique_ptr<SpriteAsset> asset = SpriteAssetLoader::loadFromFile(renderer, fullPath);
    if (asset) {
        if (asset->assetId != assetId) {
            std::cerr << "Asset ID Mismatch! ..." << std::endl;
            return nullptr;
        }
        auto [inserted_it, success] = m_spriteAssetCache.emplace(assetId, std::move(asset));
        return inserted_it->second.get();
    }
    return nullptr;
}
