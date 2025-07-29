#pragma once

#include "../core/math_types.hpp"

/**
 * @struct RigidBodyComponent
 * @brief Stores movement properties for an entity in the physics simulation.
 */
struct RigidBodyComponent {
    Vec2f velocity{0.0f, 0.0f};
    
    // We can add more physics properties here in the future.
    // Vec2f acceleration{0.0f, 0.0f};
    // float mass = 1.0f;
    // float friction = 0.1f;
};
