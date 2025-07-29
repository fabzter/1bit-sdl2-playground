#pragma once

#include "../core/math_types.hpp"
#include <cstdint>

/**
 * @struct ColliderComponent
 * @brief Defines an Axis-Aligned Bounding Box (AABB) for physics interactions.
 */
struct ColliderComponent {
    // The offset of the collider box from the entity's transform position.
    Vec2f offset{0.0f, 0.0f};
    // The size of the collider box.
    Vec2f size{0.0f, 0.0f};

    // A bitmask representing the physics layer(s) this collider is ON.
    // Example: 0b0001 might be "world", 0b0010 might be "player".
    uint32_t layer = 0;

    // A bitmask representing the physics layer(s) this collider INTERACTS WITH.
    uint32_t mask = 0;

    // If true, this collider does not move and is considered part of the static world geometry.
    bool is_static = false;

    // If true, this collider detects overlaps but doesn't cause a solid collision response.
    // Useful for detection zones, damage areas, etc.
    bool is_trigger = false;
};
