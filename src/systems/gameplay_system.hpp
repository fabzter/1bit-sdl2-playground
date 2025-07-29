#pragma once

#include "../core/systems/isystem.hpp"
#include "../events/collision.hpp"

class GameplaySystem : public IUpdateSystem {
public:
    void init(entt::registry& registry) override;
    void update(entt::registry& registry, InputManager&, ResourceManager&, float) override {}

private:
    void onCollision(const CollisionEvent& event);
    entt::registry* m_registry = nullptr;
};
