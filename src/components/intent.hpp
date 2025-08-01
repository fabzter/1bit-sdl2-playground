#pragma once

#include "transform.hpp" // For Vec2f

/**
 * @struct IntentComponent
 * @brief Stores the raw, normalized intent of a controllable entity for the current frame.
 * This acts as an intermediate data structure between input polling and movement physics.
 */
struct IntentComponent {
    Vec2f moveDirection{0.0f, 0.0f};
    bool actionJustPressed = false;
};
