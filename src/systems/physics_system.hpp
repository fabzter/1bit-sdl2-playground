#pragma once

#include "../core/systems/isystem.hpp"

class PhysicsSystem : public IUpdateSystem {
public:
    void update(entt::registry& registry, InputManager& inputManager, ResourceManager& resourceManager, float deltaTime) override;
};
