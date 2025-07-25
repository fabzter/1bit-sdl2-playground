#pragma once

#include <string>
#include <memory>
#include <SDL2/SDL.h>
#include "sprite_asset.hpp" //Re-use the SDL_Texture_Deleter TODO: refactor -

/**
 * @struct TilesetAsset
 * @brief Represents a loaded tileset, containing the texture atlas and metadata.
 */
struct TilesetAsset {
    std::string assetId;
    int tileWidth = 0;
    int tileHeight = 0;
    int tileCount = 0;
    int columns = 0;

    // A single texture containing all tiles laid out in a grid.
    std::unique_ptr<SDL_Texture, SDL_Texture_Deleter> textureAtlas;
};
