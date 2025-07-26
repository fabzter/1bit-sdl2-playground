#pragma once

#include "../core/systems/isystem.hpp"

class RenderSystem: public IRenderSystem{
public:
    RenderSystem() = default;

    void init(entt::registry& registry) override;
    void draw(SDL_Renderer* renderer, entt::registry& registry,
        ResourceManager& resourceManager) override;

private:
    // signal listener.
    void onSpriteUpdate(entt::registry& registry, entt::entity entity);
    // A persistent, sorted list of entities to render
    std::vector<entt::entity> m_renderQueue;
    // The flag to control when we re-sort
    bool m_isDirty = true;
};
