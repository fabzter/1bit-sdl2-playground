#pragma once

#include "../core/systems/isystem.hpp"

class AnimationStateSystem: public IUpdateSystem {
public:
    AnimationStateSystem() = default;
    void update(entt::registry& registry, InputManager& inputManager, ResourceManager& resourceManager,
        float deltaTime) override;;
};
