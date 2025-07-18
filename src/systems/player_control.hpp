#pragma once

#include <entt/entt.hpp>
#include "../core/input_manager.hpp"

class PlayerControlSystem {
public:
    PlayerControlSystem() = default;

    void update(entt::registry& registry, const InputManager& inputManager, float deltaTime);
};