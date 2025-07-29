#pragma once

#include "../core/math_types.hpp"

/**
 * @struct TransformComponent
 * @brief Holds data for an entity's position, scale, and rotation in 2D space.
 * This component is essential for any entity that needs to be rendered or have a physical presence.
 */
struct TransformComponent {
    Vec2f position{0.0f, 0.0f};
    Vec2f scale{1.0f, 1.0f};
    float rotation = 0.0f; // Rotation in degrees

    TransformComponent() = default;
    TransformComponent(const Vec2f& pos, const Vec2f& scl = {1.0f, 1.0f}, float rot = 0.0f)
        : position(pos), scale(scl), rotation(rot) {}
};
