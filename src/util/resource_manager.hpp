#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <SDL2/SDL.h>
#include "texture_loader.hpp"

// Forward-declare the custom deleter
struct SDL_Texture_Deleter {
    void operator()(SDL_Texture* texture) const;
};

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    // Disallow copying and moving
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    SDL_Texture* getTexture(SDL_Renderer* renderer, const std::string& assetId);
    void preloadTextures(SDL_Renderer* renderer, const std::vector<std::string>& assetIds);

private:
    std::string m_basePath;
    std::unordered_map<std::string, std::unique_ptr<SDL_Texture, SDL_Texture_Deleter>> m_textureCache;
};
