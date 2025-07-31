#pragma once

#include "../core/systems/isystem.hpp"

class CharacterControllerSystem: public IUpdateSystem {
public:
    CharacterControllerSystem() = default;

    void update(entt::registry& registry, InputManager& inputManager,
        ResourceManager& resourceManager, float deltaTime) override;
};