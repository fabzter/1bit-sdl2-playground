#pragma once

#include <entt/entt.hpp>
#include "../core/input_manager.hpp"

class PlayerIntentSystem {
public:
    PlayerIntentSystem() = default;

    void update(entt::registry& registry, const InputManager& inputManager, float deltaTime);
};
