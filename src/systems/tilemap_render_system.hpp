#pragma once

#include <entt/entt.hpp>
#include <SDL2/SDL.h>
#include "../util/resource_manager.hpp"

class TilemapRenderSystem {
public:
    TilemapRenderSystem() = default;

    void draw(SDL_Renderer* renderer, entt::registry& registry, ResourceManager& resourceManager);
};
