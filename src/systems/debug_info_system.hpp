#pragma once

#include <entt/entt.hpp>
#include "../core/input_manager.hpp"
#include "../util/resource_manager.hpp"

class DebugInfoSystem {
public:
    DebugInfoSystem() = default;

    void update(entt::registry& registry, const InputManager& inputManager, ResourceManager& resourceManager);
};
