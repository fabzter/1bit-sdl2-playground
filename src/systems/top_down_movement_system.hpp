#pragma once

#include "../core/systems/isystem.hpp"

class TopDownMovementSystem: public IUpdateSystem {
public:
    TopDownMovementSystem() = default;

    void update(entt::registry& registry, InputManager& inputManager,
        ResourceManager& resourceManager, float deltaTime) override;
};