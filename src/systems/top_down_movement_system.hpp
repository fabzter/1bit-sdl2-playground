#pragma once

#include <entt/entt.hpp>

class TopDownMovementSystem {
public:
    TopDownMovementSystem() = default;

    void update(entt::registry& registry, float deltaTime);
};