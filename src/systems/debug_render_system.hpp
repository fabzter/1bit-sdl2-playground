#pragma once

#include "../core/systems/isystem.hpp"

class DebugRenderSystem : public IRenderSystem {
public:
    void draw(SDL_Renderer* renderer, entt::registry& registry, ResourceManager& resourceManager) override;
};
