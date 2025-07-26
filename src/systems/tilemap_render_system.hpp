#pragma once

#include "../core/systems/isystem.hpp"

class TilemapRenderSystem: public IRenderSystem {
public:
    TilemapRenderSystem() = default;

    void draw(SDL_Renderer* renderer, entt::registry& registry,
        ResourceManager& resourceManager) override;
};
