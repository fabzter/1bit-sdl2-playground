#pragma once

#include "../core/systems/isystem.hpp"

class CameraSystem: public IUpdateSystem {
public:
    CameraSystem() = default;

    void update(entt::registry& registry, InputManager& inputManager,
        ResourceManager& resourceManager, float deltaTime) override;
};
