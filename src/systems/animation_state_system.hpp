#pragma once

#include <entt/entt.hpp>

class AnimationStateSystem {
public:
    AnimationStateSystem() = default;
    void update(entt::registry& registry);
};
