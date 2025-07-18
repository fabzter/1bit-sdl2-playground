#pragma once

/**
 * @struct MovementComponent
 * @brief Holds data related to an entity's movement capabilities.
 */
struct MovementComponent {
    float speed = 0.0f;

    MovementComponent() = default;
    explicit MovementComponent(float s) : speed(s) {}
};
