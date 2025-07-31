#include "collision_system.hpp"
#include "../components/transform.hpp"
#include "../components/collider.hpp"
#include "../events/collision.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

/**
 * @brief Calculates the Minimum Translation Vector (MTV) to resolve AABB overlap.
 * @return A Vec2f representing the smallest push to separate the two rectangles.
 */
static Vec2f calculateMTV(const QuadtreeRect& a, const QuadtreeRect& b) {
    float overlapX1 = (a.x + a.w) - b.x;
    float overlapX2 = (b.x + b.w) - a.x;
    float overlapY1 = (a.y + a.h) - b.y;
    float overlapY2 = (b.y + b.h) - a.y;

    float overlapX = std::min(overlapX1, overlapX2);
    float overlapY = std::min(overlapY1, overlapY2);

    if (overlapX < overlapY) {
        return { (overlapX1 < overlapX2) ? -overlapX : overlapX, 0.0f };
    } else {
        return { 0.0f, (overlapY1 < overlapY2) ? -overlapY : overlapY };
    }
}

CollisionSystem::CollisionSystem(const QuadtreeRect& worldBounds) {
    m_quadtree = std::make_unique<Quadtree<entt::entity>>(0, worldBounds);
}

// Helper function to create a QuadtreeRect from an entity's components
static QuadtreeRect getEntityBounds(const TransformComponent& transform, const ColliderComponent& collider) {
    // Calculate the final, scaled size and offset of the collider.
    const float scaledWidth = collider.size.x * transform.scale.x;
    const float scaledHeight = collider.size.y * transform.scale.y;
    const float scaledOffsetX = collider.offset.x * transform.scale.x;
    const float scaledOffsetY = collider.offset.y * transform.scale.y;

    // 1. Start with the entity's center position.
    // 2. Subtract half the scaled size to find the top-left corner.
    // 3. Add the (now optional) scaled offset for special adjustments.
    //TODO: where is the 2.0f comming from? seems like a hardcoded data taht may bring problems later
    return {
        static_cast<int>(transform.position.x - (scaledWidth / 2.0f) + scaledOffsetX),
        static_cast<int>(transform.position.y - (scaledHeight / 2.0f) + scaledOffsetY),
        static_cast<int>(scaledWidth),
        static_cast<int>(scaledHeight)
    };
}

// Helper for the AABB intersection check
static bool checkAABBCollision(const QuadtreeRect& a, const QuadtreeRect& b) {
    return (a.x < b.x + b.w && a.x + a.w > b.x &&
            a.y < b.y + b.h && a.y + a.h > b.y);
}

void CollisionSystem::dePenetrate(TransformComponent &dynamicTransform, const QuadtreeRect &dynamicBounds, const QuadtreeRect &staticBounds) {
    Vec2f mtv = calculateMTV(dynamicBounds, staticBounds);

    // 2. Apply the entire push to the dynamic object.
    dynamicTransform.position.x += mtv.x;
    dynamicTransform.position.y += mtv.y;
}

void CollisionSystem::resolveStaticCollision(TransformComponent& dynamicTransform, const QuadtreeRect& dynamicBounds,
                                             const QuadtreeRect& staticBounds) {
    dePenetrate(dynamicTransform, dynamicBounds, staticBounds);
}

void CollisionSystem::resolveDynamicCollision(
    TransformComponent& transformA, RigidBodyComponent& rbA, const QuadtreeRect& boundsA,
    TransformComponent& transformB, RigidBodyComponent& rbB, const QuadtreeRect& boundsB)
{
    // --- 1. Positional Correction (Depenetration) ---
    Vec2f mtv = calculateMTV(boundsA, boundsB);

    float totalMass = rbA.mass + rbB.mass;
    if (totalMass <= 0) return; // Avoid division by zero if both have zero mass

    // 1b. Distribute the push based on the mass ratio.
    float pushRatioA = rbB.mass / totalMass;
    float pushRatioB = rbA.mass / totalMass;
    transformA.position.x += mtv.x * pushRatioA;
    transformA.position.y += mtv.y * pushRatioA;
    transformB.position.x -= mtv.x * pushRatioB;
    transformB.position.y -= mtv.y * pushRatioB;

    // --- 2. Velocity Correction (Impulse) ---
    float combinedRestitution = std::min(rbA.restitution, rbB.restitution);
    Vec2f relativeVelocity = {rbB.velocity.x - rbA.velocity.x, rbB.velocity.y - rbA.velocity.y};
    Vec2f normal = {mtv.x, mtv.y};
    // Normalize the normal vector
    float length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
    if (length > 0) {
        normal.x /= length;
        normal.y /= length;
    }

    float velAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;
    if (velAlongNormal > 0) return; // Objects are already moving apart

    float impulseScalar = -(1 + combinedRestitution) * velAlongNormal;
    impulseScalar /= (1 / rbA.mass + 1 / rbB.mass);

    Vec2f impulse = {impulseScalar * normal.x, impulseScalar * normal.y};

    rbA.velocity.x -= (1 / rbA.mass) * impulse.x;
    rbA.velocity.y -= (1 / rbA.mass) * impulse.y;
    rbB.velocity.x += (1 / rbB.mass) * impulse.x;
    rbB.velocity.y += (1 / rbB.mass) * impulse.y;
}

//TODO: while move and depentrate is an extremely common and robust pattern used in countless games, especially for character controllers. Many systems in Godot, Unity, and custom engines use this exact logic for player movement because it elegantly handles sliding along walls and is much simpler to implement reliably than a full CCD system, this is still a work in progress to evaluate, make cleaner and more generic.
//TODO: dicuss: is the fact that we are applying here move and depenetrate valid or should it be part of another system? May be it is correct but is worh a discussion
void CollisionSystem::update(entt::registry& registry, InputManager&, ResourceManager&, float deltaTime) {
    // Get the dispatcher from the registry's context
    auto& dispatcher = registry.ctx().get<entt::dispatcher>();

    // === 1. BROAD PHASE ===
    // Clear the quadtree and re-populate it with all colliders.
    m_quadtree->clear();
    auto allCollidersView = registry.view<const TransformComponent, const ColliderComponent>();
    for (const auto entity : allCollidersView) {
        const auto& [transform, collider] = allCollidersView.get<const TransformComponent, const ColliderComponent>(entity);
        m_quadtree->insert(entity, getEntityBounds(transform, collider));
    }

    // === 2. NARROW PHASE === (with depenetration)
    // We only need to check moving objects against the quadtree.
    auto dynamicEntitiesView = registry.view<TransformComponent, RigidBodyComponent,
    const ColliderComponent>();
    for (const auto entity : dynamicEntitiesView) {
        // We only need to check DYNAMIC bodies, as static ones don't initiate collision checks.
        auto& rigidbody = dynamicEntitiesView.get<RigidBodyComponent>(entity);
        if (rigidbody.bodyType == BodyType::STATIC) continue;

        // ---(Get potential collisions from Quadtree) ---
        auto& transform = dynamicEntitiesView.get<TransformComponent>(entity);
        const auto& collider = dynamicEntitiesView.get<const ColliderComponent>(entity);

        // Get potentials collisions from Quadtree
        QuadtreeRect entityBounds = getEntityBounds(transform, collider);
        std::vector<entt::entity> potentialCollisions;
        m_quadtree->query(entityBounds, potentialCollisions);
        
        for (const auto otherEntity : potentialCollisions) {
            // Don't check an entity against itself.
            if (entity == otherEntity) {
                continue;
            }

            const auto& otherCollider = registry.get<const ColliderComponent>(otherEntity);
            // Check if the physics layers and masks allow for a collision.
            bool canCollide = (collider.mask & otherCollider.layer) && (otherCollider.mask & collider.layer);
            if (!canCollide) {
                continue;
            }

            const auto& otherConstTransform = registry.get<const TransformComponent>(otherEntity);
            const auto otherBounds = getEntityBounds(otherConstTransform, otherCollider);

            // TODO: I dont think resolution should go here? since every object should be able to react differently to collisions. Or maybe yes? this is a discussion topic.
            if (checkAABBCollision(entityBounds, otherBounds)) {
                //TODO: discuss if this component should be responsible for resolution
                dispatcher.enqueue<CollisionEvent>(entity, otherEntity);

                // Now, check if we should skip the physical resolution part.
                if (collider.is_trigger || otherCollider.is_trigger) {
                    continue;
                }

                //TODO: again im thinking this resolution should not be part of the collission system but for more specialized systems.
                // Safely try to get the other entity's rigidbody.
                auto* otherRigidbody = registry.try_get<RigidBodyComponent>(otherEntity);
                if (otherRigidbody) {
                    // --- COLLISION RESPONSE ---
                    // If the other body is STATIC OR KINEMATIC, treat it as an immovable wall.
                    if (otherRigidbody->bodyType == BodyType::STATIC ||
                        otherRigidbody->bodyType == BodyType::KINEMATIC) {
                        resolveStaticCollision(transform, entityBounds, otherBounds);
                    } else if (otherRigidbody->bodyType == BodyType::DYNAMIC) { // Both bodies are DYNAMIC.
                        auto& otherMutableTransform = registry.get<TransformComponent>(otherEntity);
                        resolveDynamicCollision(transform, rigidbody, entityBounds,
                            otherMutableTransform, *otherRigidbody, otherBounds);
                    }
                } else {
                    // The other entity does NOT have a rigidbody, so treat it as a simple static wall.
                    resolveStaticCollision(transform, entityBounds, otherBounds);
                }
            }
        }
    }
}
