#pragma once

#include <entt/entt.hpp>

class MovementSystem {
public:
    MovementSystem() = default;

    void update(entt::registry& registry, float deltaTime);
};