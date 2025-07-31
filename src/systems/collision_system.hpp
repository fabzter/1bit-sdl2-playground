#pragma once

#include "../core/systems/isystem.hpp"
#include "../util/quadtree.hpp"
#include "../components/transform.hpp"
#include "../components/rigidbody.hpp"
#include <memory>
#include <entt/entt.hpp>

class CollisionSystem : public IUpdateSystem {
public:
    // We initialize the system with the boundaries of our world.
    CollisionSystem(const QuadtreeRect& worldBounds);

    void update(entt::registry& registry, InputManager& inputManager, ResourceManager& resourceManager, float deltaTime) override;

private:
    std::unique_ptr<Quadtree<entt::entity>> m_quadtree;

    void dePenetrate(TransformComponent &dynamicTransform, const QuadtreeRect &dynamicBounds,
                       const QuadtreeRect &staticBounds);

    /**
 * @brief Resolves a collision between a dynamic entity and a static entity.
 * This performs a simple depenetration, pushing the dynamic entity out of the static one.
 */
    void resolveStaticCollision(TransformComponent& dynamicTransform, const QuadtreeRect& dynamicBounds,
        const QuadtreeRect& staticBounds);

    /**
 * @brief Resolves a collision between two dynamic entities.
 * This handles both positional depenetration and velocity changes (impulse) based on mass and restitution.
 */
    void resolveDynamicCollision(
        TransformComponent& transformA, RigidBodyComponent& rbA, const QuadtreeRect& boundsA,
        TransformComponent& transformB, RigidBodyComponent& rbB, const QuadtreeRect& boundsB);
};
