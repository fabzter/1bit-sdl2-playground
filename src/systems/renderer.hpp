#pragma once

#include "../core/systems/isystem.hpp"

class RenderSystem: public IRenderSystem{
public:
    RenderSystem() = default;

    void draw(SDL_Renderer* renderer, entt::registry& registry,
        ResourceManager& resourceManager) override;

private:
};
