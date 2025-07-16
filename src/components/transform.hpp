#pragma once

/**
 * @struct Vec2f
 * @brief A simple structure to hold 2D float coordinates.
 */
struct Vec2f {
    float x = 0.0f;
    float y = 0.0f;
};

/**
 * @struct TransformComponent
 * @brief Holds data for an entity's position, scale, and rotation in 2D space.
 * This component is essential for any entity that needs to be rendered or have a physical presence.
 */
struct TransformComponent {
    Vec2f position{0.0f, 0.0f};
    Vec2f scale{1.0f, 1.0f};
    float rotation = 0.0f; // Rotation in degrees
};