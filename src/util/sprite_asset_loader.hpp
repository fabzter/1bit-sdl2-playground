#pragma once

#include <string>
#include <SDL2/SDL.h>
#include "sprite_asset.hpp"

class SpriteAssetLoader {
public:
    static std::unique_ptr<SpriteAsset> loadFromFile(SDL_Renderer* renderer, const std::string& filepath);
};
