#pragma once

#include "../core/systems/isystem.hpp"
#include "../util/quadtree.hpp"
#include "../components/transform.hpp"
#include <memory>
#include <entt/entt.hpp>

class CollisionSystem : public IUpdateSystem {
public:
    // We initialize the system with the boundaries of our world.
    CollisionSystem(const QuadtreeRect& worldBounds);

    void update(entt::registry& registry, InputManager& inputManager, ResourceManager& resourceManager, float deltaTime) override;

private:
    std::unique_ptr<Quadtree<entt::entity>> m_quadtree;

    void dePenetrate(entt::basic_sigh_mixin<entt::basic_storage<TransformComponent>,
        entt::basic_registry<>>::value_type &transform,
        QuadtreeRect entityBounds, const QuadtreeRect otherBounds);
};
