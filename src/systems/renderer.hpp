#pragma once

#include <SDL2/SDL.h>
#include "../util/resource_manager.hpp"
#include <entt/entt.hpp>

class RenderSystem {
public:
    RenderSystem() = default;

    void draw(SDL_Renderer* renderer, entt::registry& registry, ResourceManager& resourceManager);

private:
};
