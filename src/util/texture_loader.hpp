#pragma once

#include <string>
#include <SDL2/SDL.h>

class TextureLoader {
public:
    static SDL_Texture* loadFromSpriteFile(SDL_Renderer* renderer, const std::string& filepath);
};
