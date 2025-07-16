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

SDL_Texture* ResourceManager::getTexture(SDL_Renderer* renderer, const std::string& assetId) {
    // Check if the texture is already in our cache
    auto it = m_textureCache.find(assetId);
    if (it != m_textureCache.end()) {
        return it->second.get();
    }

    // It's not cached, so let's load it.
    // We construct the full path: /path/to/executable/res/sprites/assetId.sprite
    std::string fullPath = m_basePath + "res/sprites/" + assetId + ".sprite";
    std::cout << "Loading texture from: " << fullPath << std::endl;

    SDL_Texture* rawTexture = TextureLoader::loadFromSpriteFile(renderer, fullPath);
    if (rawTexture) {
        // Store it in the cache. `emplace` is slightly more efficient than
        // operator[] as it constructs the std::unique_ptr in-place.
        m_textureCache.emplace(assetId, rawTexture);
        return rawTexture;
    }

    // Return nullptr if loading failed
    return nullptr;
}

void ResourceManager::preloadTextures(SDL_Renderer* renderer, const std::vector<std::string>& assetIds) {
    std::cout << "Preloading " << assetIds.size() << " texture(s)..." << std::endl;
    for (const auto& assetId : assetIds) {
        getTexture(renderer, assetId); // This will load and cache the texture if not already present
    }
    std::cout << "Preloading complete." << std::endl;
}
