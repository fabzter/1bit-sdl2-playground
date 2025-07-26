#pragma once

#include <entt/entt.hpp>
#include "../core/input_manager.hpp"
#include "../util/resource_manager.hpp"
#include "../core/systems/isystem.hpp"

class DebugInfoSystem: public IUpdateSystem {
public:
    DebugInfoSystem() = default;

    void update(entt::registry& registry, InputManager& inputManager,
        ResourceManager& resourceManager, float deltaTime) override;
};
