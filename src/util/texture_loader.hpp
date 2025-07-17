#pragma once

#include <string>
#include <SDL2/SDL.h>
#include "sprite_asset.hpp"

class TextureLoader {
public:
    // TODO: it makes more sense to have a system that loads all assets and read the IDs from the asset itself
    // TODO: is this the right noame for the class and method since it is returning an Asset and not a Texture?ß
    static std::unique_ptr<SpriteAsset> loadFromSpriteFile(SDL_Renderer* renderer,
        const std::string& assetId, const std::string& filepath);
};
