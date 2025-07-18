#pragma once

#include <entt/entt.hpp>
#include "../util/resource_manager.hpp"

class AnimationSystem {
public:
    AnimationSystem() = default;

    /**
     * @brief Updates the animation frame for all animated entities.
     * @param registry The scene's entity registry.
     * @param deltaTime The time elapsed since the last frame.
     * @param resourceManager The resource manager to fetch sprite assets.
     */
    void update(entt::registry& registry, float deltaTime, ResourceManager& resourceManager);
};
