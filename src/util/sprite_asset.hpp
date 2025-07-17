#pragma once

#include <string>
#include <memory>
#include <SDL2/SDL.h>

// Forward-declare the custom deleter
struct SDL_Texture_Deleter {
    void operator()(SDL_Texture* texture) const;
};

/**
 * @struct SpriteAsset
 * @brief A container for all data related to a single sprite asset.
 * This includes the SDL_Texture and metadata like its original size,
 * which are loaded from a .sprite file.
 */
struct SpriteAsset {
    std::string assetId;
    int width = 0;
    int height = 0;
    std::unique_ptr<SDL_Texture, SDL_Texture_Deleter> texture;

    // We can add more data here later, e.g., animation sequences.
};