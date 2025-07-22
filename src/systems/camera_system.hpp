#pragma once

#include <entt/entt.hpp>

class CameraSystem {
public:
    CameraSystem() = default;

    void update(entt::registry& registry, float deltaTime);
};
