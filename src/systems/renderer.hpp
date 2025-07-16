#pragma once

#include <SDL2/SDL.h>
#include "../util/resource_manager.hpp"

class RenderSystem {
public:
    RenderSystem() = default;
    ~RenderSystem();

    // The draw call is now responsible for loading the texture
    // on its first run, a real system would be more complex.
    void draw(SDL_Renderer* renderer, ResourceManager& resourceManager);

private:
};
