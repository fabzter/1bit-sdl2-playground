#pragma once

#include "../core/systems/isystem.hpp"

class TopDownKinematicMovementSystem: public IUpdateSystem {
public:
    TopDownKinematicMovementSystem() = default;

    void update(entt::registry& registry, InputManager& inputManager,
        ResourceManager& resourceManager, float deltaTime) override;
};