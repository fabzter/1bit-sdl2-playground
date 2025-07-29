#include "collision_system.hpp"
#include "../components/collider.hpp"
#include "../components/rigidbody.hpp"
#include "../events/collision.hpp"

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

void CollisionSystem::dePenetrate(entt::basic_sigh_mixin<entt::basic_storage<TransformComponent>, entt::basic_registry<>>::value_type &transform, QuadtreeRect entityBounds, const QuadtreeRect otherBounds) {
    float overlapX1 = (entityBounds.x + entityBounds.w) - otherBounds.x;
    float overlapX2 = (otherBounds.x + otherBounds.w) - entityBounds.x;
    float overlapY1 = (entityBounds.y + entityBounds.h) - otherBounds.y;
    float overlapY2 = (otherBounds.y + otherBounds.h) - entityBounds.y;

    float overlapX = std::min(overlapX1, overlapX2);
    float overlapY = std::min(overlapY1, overlapY2);

    // Depenetrate on the axis with the smallest overlap
    if (overlapX < overlapY) {
        // Push on X axis
        if (overlapX1 < overlapX2) {
            transform.position.x -= overlapX; // Push left
        } else {
            transform.position.x += overlapX; // Push right
        }
    } else {
        // Push on Y axis
        if (overlapY1 < overlapY2) {
            transform.position.y -= overlapY; // Push up
        } else {
            transform.position.y += overlapY; // Push down
        }
    }
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
    auto movingCollidersView = registry.view<TransformComponent, const ColliderComponent,
        const RigidBodyComponent>();
    for (const auto entity : movingCollidersView) {
        auto& transform = movingCollidersView.get<TransformComponent>(entity);
        const auto& collider = movingCollidersView.get<const ColliderComponent>(entity);

        QuadtreeRect entityBounds = getEntityBounds(transform, collider);
        
        std::vector<entt::entity> potentialCollisions;
        m_quadtree->query(entityBounds, potentialCollisions);
        
        for (const auto otherEntity : potentialCollisions) {
            // Don't check an entity against itself.
            if (entity == otherEntity) {
                continue;
            }

            const auto& otherCollider = allCollidersView.get<const ColliderComponent>(otherEntity);

            // Check if the physics layers and masks allow for a collision.
            bool canCollide = (collider.mask & otherCollider.layer) && (otherCollider.mask & collider.layer);
            if (!canCollide) {
                continue;
            }

            const auto& otherTransform = allCollidersView.get<const TransformComponent>(otherEntity);
            const auto otherBounds = getEntityBounds(otherTransform, otherCollider);

            // TODO: I dont think resolution should go here? since every object should be able to react differently to collisions. Or maybe yes? this is a discussion topic.
            if (checkAABBCollision(entityBounds, otherBounds)) {
                // --- EVENT DISPATCH LOGIC ---
                // Always dispatch an event on collision.
                dispatcher.enqueue<CollisionEvent>(entity, otherEntity);

                // --- RESOLUTION LOGIC ---
                // Only perform physical depenetration if NEITHER object is a trigger.
                //TODO: WHY?
                if (!collider.is_trigger && !otherCollider.is_trigger) {
                    // Collision detected, now calculate MTV to depenetrate
                    dePenetrate(transform, entityBounds, otherBounds);
                }
            }
        }
    }
}
