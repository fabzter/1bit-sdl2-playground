#pragma once

#include "../core/systems/isystem.hpp"
#include "../core/input_manager.hpp"
#include "../util/resource_manager.hpp"
#include "../core/entt_helpers.hpp"

class StateMachineSystem : public IUpdateSystem {
public:
    void update(entt::registry& registry, InputManager& inputManager, ResourceManager& resourceManager,
        float deltaTime) override;
};
