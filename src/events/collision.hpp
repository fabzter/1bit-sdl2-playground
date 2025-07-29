#pragma once

#include <entt/entt.hpp>

/**
 * @struct CollisionEvent
 * @brief Dispatched by the CollisionSystem when two colliders overlap.
 */
struct CollisionEvent {
    entt::entity a;
    entt::entity b;
};
