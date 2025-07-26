#pragma once

#include "../core/systems/isystem.hpp"
#include "../util/resource_manager.hpp"

class AnimationSystem: public IUpdateSystem {
public:
    AnimationSystem() = default;

    /**
     * @brief Updates the animation frame for all animated entities.
     * @param registry The scene's entity registry.
     * @param inputManager The input manager to handle user input.
     * @param resourceManager The resource manager to fetch sprite assets.
     * @param deltaTime The time elapsed since the last frame.
     */
    void update(entt::registry& registry, InputManager& inputManager, ResourceManager& resourceManager,
        float deltaTime) override;
};
