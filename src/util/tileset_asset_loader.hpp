#pragma once

#include <string>
#include <SDL2/SDL.h>
#include "tileset_asset.hpp"

class TilesetAssetLoader {
public:
    static std::unique_ptr<TilesetAsset> loadFromFile(SDL_Renderer* renderer, const std::string& filepath);
};
