#pragma once

#include "../core/input_manager.hpp"
#include "../core/systems/isystem.hpp"

class PlayerIntentSystem: public IUpdateSystem {
public:
    PlayerIntentSystem() = default;

    void update(entt::registry& registry, InputManager& inputManager,
        ResourceManager& resourceManager, float deltaTime) override;
};
